系统要求:ubuntu18.04 desktop/servier
配置步骤:
    授权问题: /etc/ 目录下新建nirvana.conf文件，文件中添加 /tmp。或把本文件下的nirvana.conf拷贝到/etc/下
    动态查找路径配置: LD_LIBRARY_PATH 中添加当前目录(.)。或在/etc/ld.so.conf中添加当前目录(.)
                    (别忘了执行　ldconfig命令)
环境测试:
    FingerQualityPro为测试工具
    测试方法：
    $ ./FingerQualityCtrlTest ./FQNet_model/deploy.prototxt ./FQNet_model/_iter_50000.caffemodel ./SourceBmpLinuxTest36 3 8
    后两个数字是模式数和 线程数
    如果在QualityBmpLinux、SourceBmpLinux　文件夹下分别有质量图和原图生成，且终端有分数输出，则环境正常

质量平台动态库:
    调用libQualityCtrl.so动态库，接口使用方法在CFIQ.h中有详细说明
