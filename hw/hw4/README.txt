

Hw4


(1)setup
	1.reverse park
		(a) 有稍微改動 bbcar.cpp 的turn 部分，使轉動時候一個輪子不動
		(b) main.cpp 可以拿來 compile
		(c) car_control.py 當中輸入給 main.cpp 的指令格式是 d1,d2,方位
		    ，ex. 15,17,east，其中距離單位是cm，並且為距離停車格最近的那點
	2. Line follow
		(a) main.cpp 可以拿來 compile
		(b) line_follow.py 的code 給 openmv 的 main.py 使用
		(c) 鏡頭可以往下轉來斜下看著地板，線需要畫粗一點，不然環境的其他黑色顏色的東西會干擾
	3. Position calib
		(a) main.cpp 可以拿來 compile
		(b) position_calib.py 的code 給 openmv 的 main.py 使用
		(c) 最後的 ultrasound ping 可能會被線擋到，所以需要調整一下



(2)results
	1.reverse park
		(a) 90度轉動的時候有時會不太給力，左輪容易卡，可能訊號可以給大點，或是改factor
		(b) 因為我是讓面對west以外的方位都修正到面對west，所以可以視停車格狀況改動在面對 west 時候倒車的距離
	2. Line follow
		(a) 鏡頭的RGB似乎不太正常，黑色會辨別成G，白色會辨別成B，所以比較沒辦法用其他顏色來標示 line，所以可能需要找干擾比較少的地方做
	3. Position calib
		(a) 可以根據 rotatY_LeftMin 跟 rotatY_RightMax 調整希望有多正對著 apriltag，否則可能車子需要一段時間來校正
		(b) TooClose 參數也可以設定車子在多少距離會太靠近apriltag，需要後退
		