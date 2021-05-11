

指令 
	/gesture/run  
	/tilt/run
手勢
	ring
	slope 
	“把板子平放在桌上左右滑” 的 left right

主要運行的檔案
	main.cpp
	rpc_function.py


備註
	mbed 常常會不太穩定，所以常常需要 RESET 或者 重新接上板子 或者 重新打指令
	手勢辨識有時會需要重做幾次
	rpc_function.py 就在想要 publish 前先開就好
	理論上可以想打幾次指令就打幾次，也就是ex. 可以重複打 /gesture/run，但是有可能會不穩定


理想上的運作模式
	可以先 RESET
	看到連上到熱點的訊息後
	打 /gesture/run 
	進入gesture UI mode，亮黃燈，uLCD顯示 mode 以及 angle x 代表根據手勢辨別角度 ，以及LED1亮 代表可以開始選角度
	上面會根據手勢選擇 30，45，60 度 顯示在 uLCD 上 
	選好後按下 USER BUTTON 就會 publish 給 rpc_function.py（rpc_function.py在之前先打開就行）
	rpc_functionn.py 就會送 /tilt/run 給 mbed
	進入 tilt mode，亮藍燈，uLCD會顯示 mode，並且一直顯示tilt angle
	如果板子沒有 “讓 RESET 朝右 以及 水平放置”，uLCD 上就會顯示uninitialized，LED1 也不會亮
	完成上述後，uLCD 顯示 initialized ， LED1亮， 代表開始做 “角度超過” 的偵測
	角度超過就會 publish 給 rpc_function.py，上面會顯示第幾次的 “角度超過”
	超過 5 次的 “角度超過” 後 rpc_function.py 就會送/gesture/run 給 mbed
	重複做 /gesture/run
	

	