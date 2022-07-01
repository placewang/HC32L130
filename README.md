**22.6.10添加了动态修改波特率**  
**22.6.12支持串口升级**  
**22.6-15 修改了Flash写机制 规避芯片大量写数据错误缺陷**  
**22.6-28 修改了与主控的交互的超时时间**  
**22.6-29 修改了升级标志写入FLASH地址**  
**22.6-29 添加了启动文件汇编如下**
```s
		WHILE . <0x500
                DCD     0xFFFFFFFF
                WEND
                IF      :LNOT:(:DEF:NDEBUG)
FLAGS           DCD     0xA05055AA  ; 0xFFFFFFFF  ; 0x55AA55AA
                ELSE
FLAGS           DCD     0xFFFFFFFF
                ENDIF
```
**22.7-1 修改了软件重启方式（饿死看门狗）NVIC_SysRest()函数重启无法关闭看门狗**  
