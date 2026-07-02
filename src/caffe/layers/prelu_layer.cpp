#include <algorithm>
#include <vector>

#if defined(__aarch64__)
#include <arm_neon.h>
#endif

#include "caffe/filler.hpp"

#include "caffe/layers/neuron_layer.hpp"
#include "caffe/layers/prelu_layer.hpp"

namespace caffe {

template <typename Dtype>
void PReLULayer<Dtype>::LayerSetUp(const vector<Blob<Dtype>*>& bottom,
    const vector<Blob<Dtype>*>& top) {
  //CHECK_GE(bottom[0]->num_axes(), 2)
  //    << "Number of axes of bottom blob must be >=2.";
  PReLUParameter prelu_param = this->layer_param().prelu_param();
  int channels = bottom[0]->channels();
  channel_shared_ = prelu_param.channel_shared();
  if (this->blobs_.size() > 0) {
   // LOG(INFO) << "Skipping parameter initialization";
  } else {
    this->blobs_.resize(1);
    if (channel_shared_) {
      this->blobs_[0].reset(new Blob<Dtype>(vector<int>(0)));
    } else {
      this->blobs_[0].reset(new Blob<Dtype>(vector<int>(1, channels)));
    }
    shared_ptr<Filler<Dtype> > filler;
    if (prelu_param.has_filler()) {
      filler.reset(GetFiller<Dtype>(prelu_param.filler()));
    } else {
      FillerParameter filler_param;
      filler_param.set_type("constant");
      filler_param.set_value(0.25);
      filler.reset(GetFiller<Dtype>(filler_param));
    }
    filler->Fill(this->blobs_[0].get());
  }
  if (channel_shared_) {
    //CHECK_EQ(this->blobs_[0]->count(), 1)
    //    << "Negative slope size is inconsistent with prototxt config";
  } else {
    //CHECK_EQ(this->blobs_[0]->count(), channels)
    //    << "Negative slope size is inconsistent with prototxt config";
  }

  // Propagate gradients to the parameters (as directed by backward pass).
  this->param_propagate_down_.resize(this->blobs_.size(), true);
  multiplier_.Reshape(vector<int>(1, bottom[0]->count(1)));
  backward_buff_.Reshape(vector<int>(1, bottom[0]->count(1)));
  caffe_set(multiplier_.count(), Dtype(1), multiplier_.mutable_cpu_data());
}

template <typename Dtype>
void PReLULayer<Dtype>::Reshape(const vector<Blob<Dtype>*>& bottom,
    const vector<Blob<Dtype>*>& top) {
  //CHECK_GE(bottom[0]->num_axes(), 2)
  //    << "Number of axes of bottom blob must be >=2.";
  top[0]->ReshapeLike(*bottom[0]);
  if (bottom[0] == top[0]) {
    // For in-place computation
    bottom_memory_.ReshapeLike(*bottom[0]);
  }
}

template <typename Dtype>
void PReLULayer<Dtype>::Forward_cpu(const vector<Blob<Dtype>*>& bottom,
    const vector<Blob<Dtype>*>& top) {
  const Dtype* bottom_data = bottom[0]->cpu_data();
  Dtype* top_data = top[0]->mutable_cpu_data();
  const int count = bottom[0]->count();
  const int dim = bottom[0]->count(2);
  const int channels = bottom[0]->channels();
  const Dtype* slope_data = this->blobs_[0]->cpu_data();

  // For in-place computation
  if (bottom[0] == top[0]) {
    caffe_copy(count, bottom_data, bottom_memory_.mutable_cpu_data());
  }

  if (channel_shared_) {
    const Dtype slope = slope_data[0];
#if defined(__aarch64__)
    if (sizeof(Dtype) == sizeof(float)) {
      const float* bottom_float = reinterpret_cast<const float*>(bottom_data);
      float* top_float = reinterpret_cast<float*>(top_data);
      const float slope_float = static_cast<float>(slope);
      const float32x4_t zero = vdupq_n_f32(0.0f);
      const float32x4_t slope_vec = vdupq_n_f32(slope_float);
      int i = 0;
      for (; i + 4 <= count; i += 4) {
        const float32x4_t value = vld1q_f32(bottom_float + i);
        const uint32x4_t positive = vcgtq_f32(value, zero);
        const float32x4_t scaled = vmulq_f32(value, slope_vec);
        vst1q_f32(top_float + i, vbslq_f32(positive, value, scaled));
      }
      for (; i < count; ++i) {
        const float value = bottom_float[i];
        top_float[i] = value > 0.0f ? value : slope_float * value;
      }
      return;
    }
#endif
    for (int i = 0; i < count; ++i) {
      const Dtype value = bottom_data[i];
      top_data[i] = value > Dtype(0) ? value : slope * value;
    }
    return;
  }

  const int num = bottom[0]->shape(0);
  for (int n = 0; n < num; ++n) {
    const int num_offset = n * channels * dim;
    for (int c = 0; c < channels; ++c) {
      const Dtype slope = slope_data[c];
      const int offset = num_offset + c * dim;
#if defined(__aarch64__)
      if (sizeof(Dtype) == sizeof(float)) {
        const float* bottom_float = reinterpret_cast<const float*>(bottom_data + offset);
        float* top_float = reinterpret_cast<float*>(top_data + offset);
        const float slope_float = static_cast<float>(slope);
        const float32x4_t zero = vdupq_n_f32(0.0f);
        const float32x4_t slope_vec = vdupq_n_f32(slope_float);
        int d = 0;
        for (; d + 4 <= dim; d += 4) {
          const float32x4_t value = vld1q_f32(bottom_float + d);
          const uint32x4_t positive = vcgtq_f32(value, zero);
          const float32x4_t scaled = vmulq_f32(value, slope_vec);
          vst1q_f32(top_float + d, vbslq_f32(positive, value, scaled));
        }
        for (; d < dim; ++d) {
          const float value = bottom_float[d];
          top_float[d] = value > 0.0f ? value : slope_float * value;
        }
        continue;
      }
#endif
      for (int d = 0; d < dim; ++d) {
        const int index = offset + d;
        const Dtype value = bottom_data[index];
        top_data[index] = value > Dtype(0) ? value : slope * value;
      }
    }
  }
}

//template <typename Dtype>
//void PReLULayer<Dtype>::Backward_cpu(const vector<Blob<Dtype>*>& top,
//    const vector<bool>& propagate_down,
//    const vector<Blob<Dtype>*>& bottom) {
//  const Dtype* bottom_data = bottom[0]->cpu_data();
//  const Dtype* slope_data = this->blobs_[0]->cpu_data();
//  const Dtype* top_diff = top[0]->cpu_diff();
//  const int count = bottom[0]->count();
//  const int dim = bottom[0]->count(2);
//  const int channels = bottom[0]->channels();
//
//  // For in-place computation
//  if (top[0] == bottom[0]) {
//    bottom_data = bottom_memory_.cpu_data();
//  }
//
//  // if channel_shared, channel index in the following computation becomes
//  // always zero.
//  const int div_factor = channel_shared_ ? channels : 1;
//
//  // Propagte to param
//  // Since to write bottom diff will affect top diff if top and bottom blobs
//  // are identical (in-place computaion), we first compute param backward to
//  // keep top_diff unchanged.
//  if (this->param_propagate_down_[0]) {
//    Dtype* slope_diff = this->blobs_[0]->mutable_cpu_diff();
//    for (int i = 0; i < count; ++i) {
//      int c = (i / dim) % channels / div_factor;
//      slope_diff[c] += top_diff[i] * bottom_data[i] * (bottom_data[i] <= 0);
//    }
//  }
//  // Propagate to bottom
//  if (propagate_down[0]) {
//    Dtype* bottom_diff = bottom[0]->mutable_cpu_diff();
//    for (int i = 0; i < count; ++i) {
//      int c = (i / dim) % channels / div_factor;
//      bottom_diff[i] = top_diff[i] * ((bottom_data[i] > 0)
//          + slope_data[c] * (bottom_data[i] <= 0));
//    }
//  }
//}


#ifdef CPU_ONLY
STUB_GPU(PReLULayer);
#endif

INSTANTIATE_CLASS(PReLULayer);
REGISTER_LAYER_CLASS(PReLU);

}  // namespace caffe
