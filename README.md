## 远程协助主工程代码

** 需要注意的点：

1.工程里集成了一个32位so，可以在64位机器上进行调试

2.该工程内有dialog弹窗，在android6.0及以上会有权限问题

3.工程加上system权限并签名后，依然不会显示弹窗，酷开系统做了限制，需要预置应用才允许

4.以往调试步骤是先将内置apk重命名或删除，重启；调试apk加上权限并签名，安装，即可使用，但由于有步骤3的限制，该调试步骤作废

5.现在是保持内置apk不变动，调试apk加上权限并签名，覆盖安装，即可使用
