

目前做的是feature部份，把 data 餵給 model 辨識出手勢還沒

檔案有
	main.cpp
	rpc_function.py

main.cpp
	mode 分成 retrieve 跟 capture


	capture（要手打 /capture/run ）
		USER_BUTTON 按下到放開的期間收集資料到 captureData 的 array
		每放開 USER_BUTTON 就會 publish 第幾次的收集資料（總共會10次然後rpc_function.py送 /retrieve/run）
		並且每次放開會analyze，feature 有 1，2，3，分別是 角度卡在30~60 是 <10，10~20，>20
	Retrieve
		就publish feature 的 array 給 rpc_function.py

rpc_function.py
	還沒做完 plot
