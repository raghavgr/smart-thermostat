import time

#hysterisis
deltaOffset = 4.9

ll = 0
lt = 0
while(1):
	time.sleep(2)
	fp = open("temp.log","rb")
	buff = fp.read()
	fp.close()
	buff = buff.split("\n")
	TControl = 9999
	for line in buff:
		if("Threshold" in line):
			TControl = float(line.split(" ")[2])
		if("F : " in line):
			CurrentT = float(line.split(" ")[-1])
	if(ll != TControl or lt != CurrentT):
		print("Current Control limit : " + str(TControl))
		print("Current Temperature : " + str(CurrentT))
		if(TControl <= CurrentT):
			print("Coolant On")
			try:
				fp = open("state.log","w+")
				fp.write("on")
				fp.close()
			except:
				continue
		else:
			print("Coolant Off")
			try:
				fp = open("state.log","w+")
				fp.write("off")
				fp.close()
			except:
				continue
		ll = TControl
		lt = CurrentT
