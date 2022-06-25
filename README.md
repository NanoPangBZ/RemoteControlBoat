# 2022年机创项目(已结束)

## 1、硬件说明(机械创新实验室成员使用船只演示时必看!!!)

​	船核心板与船驱动板之间的信号线连接采用MX1.25（4Pin 8Pin都有），虽然驱动板上忘记写标志了，对齐了连接即可。驱动板P3口是给核心板供电（5V）的，核心板的P2、P4是3.3V电源，千万别接5V，不然直接GG!!

​    模块接口主要包括MPU6050、nRF24L01和七脚OLED12864，具体连接自己研究吧，这个应该简单。

​    **本次比赛中电机等的连接**

​    因为船核心板的单片机使用的是自己老项目的拆机芯片，TIM4CH1的出波有些问题，使用PWM模式能出波，但是这个管脚的PWM峰值远远低于3.3V,无法使用这个信号驱动舵机电调等，所以硬件上没有连接！

​    因为这个原因，原定使用 TIM4 CH1~CH4 做为电调控制的引脚 **改为** TIM8 CH1~CH4。(注意：驱动板上TIM8口标的是舵机驱动舵机接口，但是软件上并没有使用TIM8控制舵机，而是使用TIM8控制电调。TIM4的CH1和CH4没有使用,CH2和CH3用于控制舵机。)

​	主推进无刷电机使用电调控制，电调分别连接TIM8的CH1和CH2，这种电调和其他电调的区别是它用的香蕉头！不要搞错了，另一种电调用来驱动转向电机的，它无法提供主推进无刷电机的电流!!然后主推进电机的电调是可调油门的双向电调，我已经油门行程校准过了，建议看不懂程序就别瞎校准油门行程了！！转向无刷电机的电调它是不可调行程的电调，没有油门行程校准的功能(使用TIM8的CH3和CH4)。

​	**遥控器**

​	没啥好讲的，Type-C接口有根线忘记连接了，电脑无法识别到CH340就把Type-C翻一面连接。

## 2、软件说明

​	自己看代码注释去，懒得BBNN。没学RTOS也能看，底层分的很清除的。

​	**boat工程:**

​	bsp_pwm.c/.h -> 船核心板输出PWM

​	bsp_spi.c/.h -> SPI通讯 用于2.4G通讯模块 和 OLED屏幕 (注意:没有初始化代码)

​	bsp_usart.c/.h -> 串口,部分使用的DMA发送,自己看

​	bsp_timer.c/.h -> 代码计时器，可用于计算某些代码的运行时长

​	

## 3、写在最后

​	没必要老是跟着船走，你可以自己设计一块其它功能的驱动板，调用我的底层去控制其它东西，霍尔啊啥的。驱动板注意做好隔离，别把核心板烧了！！那块板子上的单片机是我自己的！！！！！