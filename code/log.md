freeRTOS下通过一个任务函数使用不同的任务参数建立多个任务:

错误示范:

![image-20220317214652579](log.assets/image-20220317214652579.png)

![image-20220317214411970](log.assets/image-20220317214411970.png)

ER_is->任务参数,函数内定义的局部变量

不能使用同一ER_Type变量建立多个任务,因为TaskCreate函数传入的是地址!相当于多个任务使用了同一个参数

Pass:

![image-20220317215052865](log.assets/image-20220317215052865.png)

![image-20220317215042371](log.assets/image-20220317215042371.png)

