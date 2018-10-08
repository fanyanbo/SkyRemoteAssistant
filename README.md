## 远程协助主工程代码

*需要注意的点：*

1.工程内集成了一个32位so:libfake.so, 可在64位机器上进行调试
*也可以不用该so，编译出32位的libcoocaavnc.so代替libfake.so即可

2.工程内有dialog弹窗交互，在android6.0及以上会有权限问题，弹窗无法显示
*在酷开系统中需要system权限，并签名*

3.AndroidManifest.xml加上system权限，并签名，但依然无法显示弹窗，这里酷开系统做了限制，需要预置应用才允许

*远程是内置应用*

4.以往调试步骤：1.先将内置apk重命名或删除，重启；2.调试apk加上权限并签名，安装，即可使用，但由于有上述第3点的限制，该调试步骤作废，不再采用

5.目前可行的解决方案是保持内置apk不变动，调试apk加上权限并签名，覆盖安装即可使用

6.在该工程中添加了C++层代码，并能通过编译，rebuild project即可

*在Android studio中需要配置ndk路径，ndk版本是r14,单独下载的*

7.编译后的libcoocaavnc.so在build/intermediates/ndkBuild下，将该so拷贝到src/main/jniLibs下，同时关闭主工程build.gradle中ndk编译代码，否则每次都会去编译so，但事实上我们不用，C++代码改动很少，需要编译时在打开build.gradle中的相应代码；另一个原因是在Application.mk中依赖了stl的so，会编译生成libgnustl.so,这个so会一起打包到apk中，增加了apk体积

