


1. Setup
	a. 器具
		(1) xbee 接 D10,D9
		(2) openmv 接 D1,D0
		(3) ping 接 D13
		(4) pc 可以開 sudo screen /dev/ttyACM0 看 main.cpp 的回傳 state
	b. Code
		(1) main.cpp
		(2) openmv.py 複製到 openmv 的 main.py

2. Results
	a. Sudo screen 的回傳訊息
		(1) STATE_START：	沿著線走
		(2) STATE_TAG1：		校正（0～10度或360～350度，太近會後退）
		(3) STATE_GET_PING：	量測與apriltag的距離
		(4) STATE_CIRCLE：	繞著apriltag走
		(5) STATE_TAG2：		校正（0～10度或360～350度，太近會後退）
		(6) STATE_END：		結束
		(7) STATE_START 下讀到的線的坐標
		(8) STATE_TAG1＆STATE_TAG2 的 apriltag的 translationX，translationZ，rotationY
		(9) STATE_GET_PING 下的 ping值以及取完平均的值
	b. 以下為碰到的問題以及想到的解決辦法
		(1) Xbee 從mbed回傳給 pc 不穩定（ lab11 demo 也有一點）=> 改用usb線傳給 pc 
		(2) Openmv 沒辦法辨別特定顏色的線 => 鏡頭轉向地板
		(3) Apriltag 放在地板辨識效果不太好 => 鏡頭轉成水平
		(4) Encoder 不準 => 秒速 ＆ ThisThread::sleep_for()