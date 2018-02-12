import cv2 
import sys, os
import numpy as np
import csv
import googlemaps
from datetime import datetime
import responses
import pandas as pd
import decimal as Decimal
import math

def threshold(png):
	image = cv2.imread(png)
	output = np.zeros((1500,1500,3), np.uint8)

	#blue 140,0,0
	#light red 0,0,255
	#dark red 0,0,128
	#light blue	220,255,0
	#yellow 15,255,255

	for x in range (0, 1500):
		for y in range (0, 1500):
		
			px = image[x,y]
			
			blueDelta = abs(255 - px[0]) + abs(0 -px[1]) + abs(0 - px[2])
			lrDelta = abs(0- px[0]) + abs(0 -px[1]) + abs(255 - px[2])
			lbDelta = abs(0 - px[0]) + abs(255 -px[1]) + abs(0 - px[2])
			yellowDelta = abs(255- px[0]) + abs(255 -px[1]) + abs(255 - px[2])
			purpleDelta = abs(0 - px[0]) + abs(0 -px[1]) + abs(0 - px[2])
			
			l = (blueDelta, lrDelta , lbDelta, yellowDelta, purpleDelta)
			max = l.index(min(l))
			
			if(max == 0):				
				output[x,y] = [255,0,0]
			elif (max == 1):					
				output[x,y] = [0,0,255]		
			elif (max == 2):					
				output[x,y] = [0,255,0]			
			elif (max == 3):
				output[x,y] = [255,255,255]	
			elif (max == 4):
				output[x,y] = [0,0,0]		
			else:
				print(px)
				pass

		
				
				
	cv2.imwrite('vegetation2.png',output)

def img2csv(path):
	image = cv2.imread(path)
	with open("data.csv", 'w', newline="\n", encoding="utf-8") as out:
		writer = csv.writer(out)
		image = cv2.imread(path)
		for x in range (0, 1500):
			for y in range (0, 1500):
				px = image[x,y]
				if(px[0] == 140):
					str = "see"
				elif(px[0] == 0 and px[2] == 255):
					str= "dense"
				elif(px[0] == 164):
					str= "city"
				elif(px[0] == 220):
					str= "sparse"
				elif(px[0] == 15):
					str= "normal"
				else:
					str = "?"
					print(x,y,px)
					
				writer.writerow((x,y,str))
				
def fetchAltitude(lat, long, file):
	output = np.zeros((375,375,3), np.float32)	
	gmaps = googlemaps.Client(key='AIzaSyASs6mPR1hXXDJWHixD4o3OLdskbMmcDF4')
	la1=37.28307
	lo1=23.09126
	la2=37.28307
	lo2=23.17086
	str=""
	path = [(la1, lo1), (la1, lo2)]
	for i in range (0, 375):
		
		responses.add(responses.GET,'https://maps.googleapis.com/maps/api/elevation/json', body='{"status":"OK","results":[]}', status=200,content_type='application/json')

		results = gmaps.elevation_along_path(path, 375)
		for idx,l in enumerate(results):
			print(i,idx, l)
		
		la1 -= 0.0002	
		path = [(la1, lo1), (la1, lo2)]
		
def fexp(number):
    (sign, digits, exponent) = Decimal(number).as_tuple()
    return len(digits) + exponent - 1

def fman(number):
    return Decimal(number).scaleb(-fexp(number)).normalize()		

def alt2img(txt,png):
	output = np.zeros((375,375,3), np.float32)	
	with open(txt) as input:
		for line in input:
			if(line.split( )[3].startswith('-') or float(line.split( )[3][:-1]) < 0.5):
				output[int(line.split( )[0]),int(line.split( )[1])] = [0,0,0]
			elif( int(line.split( )[0])*4 >1000 and int(line.split( )[1])*4 > 1000):	
				output[int(line.split( )[0]),int(line.split( )[1])] = [0,0,0]
			else:
				str = line.split( )[3]
				str = str[:-1]
				alt = float(str)
				
				mantisa = str.split('.')[0]
				exponent = "0"
				
				exponent = str.split('.')[1]
				exponent = exponent[0:2]
					
					
				output[int(line.split( )[0]),int(line.split( )[1])] = [float(mantisa),float(exponent),0]

				
	output = cv2.resize(output, (0,0), fx=4, fy=4) 
	cv2.imshow("a",output)
	cv2.waitKey(0)
	cv2.destroyAllWindows()
	print(output[500][500][0])
	print("%.2f" % output[500][500][1])
	cv2.imwrite(png,output)
	image = cv2.imread(png)
	print(image[500][500][0])
	print(image[500][500][1])
	
def appendAlt(data,data2):
	with open(data, 'r') as inp, open(data2, 'r') as inp2, open("data3.csv", 'w') as out:
		for x, y in zip(inp, inp2):
			x = x.strip()
			y = y.strip()
			str = x+","+y+"\n"
			str=str.replace(",",", ")
			#print(str.split( )[2],str.split( )[3])
			if(str.split( )[2] == "see," and str.split( )[3] != "0"):
				print(str.split( )[2],str.split( )[3])
				str.split( )[3] = "0"
				
			
			out.write(str)
			#print(str)
			
def img2csv(png,csvFile):
	image = cv2.imread(png)	
	with open(csvFile,'w', newline="\n", encoding="utf-8") as inp:
		writer = csv.writer(inp)
		for x in range (0, 1500):
			for y in range (0, 1500):
				px = image[x,y]
				writer.writerow([px[0]])

def black2medium(file,output):
	image = cv2.imread(file)
	outputImg = np.zeros((1500,1500,3), np.float32)	
	for x in range (0, 1500):
		for y in range (0, 1500):
			px = image[x,y]
			if(px[0] == 0 and px[1]==0 and px[2]==0):
				outputImg[x,y] = [255,0,0]
			elif (px[0]==px[1]==px[2] == 50):
				outputImg[x,y] = [0,0,0] 
			else:
				outputImg[x,y] = px;
	cv2.imwrite(output,outputImg)			
			
def veg2data(png,inp):
	image = cv2.imread(png)			
	with open(inp, 'r', newline="\n", encoding="utf-8") as input, open("alldata.txt", 'w', newline="\n", encoding="utf-8") as output:
		counter = 0
		for line in input:
			x = counter % 1500
			y = counter // 1500
			px = image[x,y]
			if (px[0]==255 and px[1]==0 and px[2]==0):			#blue
				str = line.split( )[0] + " " + line.split( )[1] + " " + line.split( )[2] + " " + line.split( )[3] +" trees"
			elif (px[0]==0 and px[1]==255 and px[2]==0):		#green
				str = line.split( )[0] + " " +  line.split( )[1] + " " + line.split( )[2] + " " + line.split( )[3] + " bushes"
			elif (px[0]==0 and px[1]==0 and px[2]==237):		#red
				str = line.split( )[0] + " " +  line.split( )[1] + " " + line.split( )[2] + " " + line.split( )[3] + " field"
			else:
				#print(x,y)
				str = 	str = line.split( )[0] + " " +  line.split( )[1] + " " + line.split( )[2] + " " + line.split( )[3] + " none"
				if(line.find("sparse") != -1 or line.find("normal") != -1 or line.find("dense") != -1 ):
					str = line.split( )[0] + " " +  line.split( )[1] + " " + line.split( )[2] + " " + line.split( )[3] + " bushes"
				
			if(line.find("sea") != -1):
				#print(x,y)
				str =  line.split( )[0] + " " +  line.split( )[1] + " " + line.split( )[2] + " " + line.split( )[3] + " none"
				
			output.write(str+"\n")
			counter+=1

def data2png(csv,png):
	output = np.zeros((1500,1500,3), np.uint8)
	with open(csv, 'r', newline="\n", encoding="utf-8") as input:
		for line in input:
			x = int(line.split( )[0])
			y = int(line.split( )[1])
			density = line.split( )[2]
			if(density == "sparse"):
				px = [50,0,0]
			elif(density == "normal"):
				px = [150,0,0]
			elif(density == "dense"):
				px = [250,0,0]	
			else:
				px = [0,0,0]	
				
			output[x,y] = px
		
	small = cv2.resize(output, (0,0), fx=0.5, fy=0.5) 	
	

	with open(csv, 'r', newline="\n", encoding="utf-8") as input:
		for line in input:
			x = int(line.split( )[0])
			y = int(line.split( )[1])
			alt = int(line.split( )[3])
			px = [0,alt,0]
			output[x,y] = output[x,y] + px
	
	output3 = np.zeros((1500,1500,3), np.uint8)
	with open(csv, 'r', newline="\n", encoding="utf-8") as input:
		for line in input:
			x = int(line.split( )[0])
			y = int(line.split( )[1])
			type = line.split( )[4]
			if(type == "field"):
				px = [0,0,50]
			elif(type == "bushes"):
				px = [0,0,150]
			elif(type == "trees"):
				px = [0,0,250]	
			else:
				px = [0,0,0]	
				
			output[x,y] = output[x,y] + px

	
	small = cv2.resize(output, (0,0), fx=0.5, fy=0.5) 
	cv2.imshow("density",small)
	cv2.waitKey(0)
	cv2.destroyAllWindows()
		
def all2csv(densityPNG,altPNG,vegetationPNG,csv):
	counter=0
	x=0
	y=0
	density = cv2.imread(densityPNG)
	altitude = cv2.imread(altPNG)
	vegetation = cv2.imread(vegetationPNG)
		
	with open(csv, 'w', newline="\n", encoding="utf-8") as output:	
		for counter in range (0,1500*1500):
			x=counter//1500
			y=counter%1500
			px = density[y][x]
			px2 = altitude[y][x]
			px3 = vegetation[y][x]
			str2= str3 = str1=""
			
			str2 = str(px2[0]) +"." +str(px2[1])
			
			
			if(px[0] == 140 and px[1] == 0 and px[2] == 0):
				str1 = "sea"
			elif (px[0] == 164 and px[1] == 73 and px[2] == 163):
				str1 = "city"
			elif (px[0] == 0 and px[1] == 0 and px[2] == 255):
				str1 = "dense"
			elif (px[0] == 15 and px[1] == 255 and px[2] == 255):
				str1 = "normal"	
			elif (px[0] == 220 and px[1] == 255 and px[2] == 0):
				str1 = "sparse"
			else:
				print("ERROR ", x,y, px[0], px[1], px[2])
				str1="normal"
			
			if(str1=="sea"):
				str2="0"
			else:
				if(px2[0] <1 ):
					str2="1"
			
			
				
			if(px3[0] == 0 and px3[1] == 0 and px3[2] == 0):
				str3 = "none"
			elif (px3[0] == 164 and px3[1] == 73 and px3[2] == 163):
				str3 = "city"
			elif (px3[0] == 0 and px3[1] == 0 and px3[2] == 237):
				str3 = "field"
			elif (px3[0] == 0 and px3[1] == 255 and px3[2] == 0):
				str3 = "trees"	
			elif (px3[0] == 255 and px3[1] == 0 and px3[2] == 0):
				str3 = "bushes"	
			else:
				print("ERROR3 ", x,y, px3[0], px3[1], px3[2])
				str3="none"	
				
			if(str1=="sea"):
				str3="none"	
			else:
				if(str3=="none"):
					str3="bushes"
			
			output.write(str(x) + " " + str(y) + " " + str1 + " " + str2 +  " " + str3 + "\n")
		
def angle():
	for angleDiff in range (0, 360):
		print(angleDiff,math.exp(0.045*9)*math.exp(9*0.131*(math.cos(angleDiff*3.1415926/180)-1)))
	

def mergePics(folder):
	image0 = cv2.imread(folder+"/output0.ppm")
	outputTotal = np.zeros((1500,1500,3), np.int32)
	for x in range (0,1500):
		for y in range (0, 1500):
			px = image0[x][y]
			outputTotal[x][y]=px

	output = np.zeros((1500,1500,3), np.int32)
	for filename in os.listdir(folder):
		image = cv2.imread(folder+"/"+filename)
		for x in range (0,1500):
			print(x)
			for y in range (0, 1500):
				#print(image[x][y])
				px = image[x][y]
				pxo = output[x][y]
				if (px[0] == 0 and px[1] == 0 and px[2] == 0):
					pxo += [10,10,10]
				image[x][y]=pxo

	cv2.imshow("burn",image)
	cv2.waitKey(0)
	cv2.destroyAllWindows()	
				
	for x in range (0,1500):
		for y in range (0, 1500):
			px = output[x][y]
			if(px[0] == 0 and px[1] == 0 and px[2] == 0):
				pass
			else:
				px2 = [0,0,px[0]]
				outputTotal[x][y]=px2
				
	cv2.imshow("burn",image)
	cv2.waitKey(0)
	cv2.destroyAllWindows()	
	cv2.imwrite("outputTotal.png",outputTotal)
		
#threshold("vegetation3.png")	
#img2csv("output.png")
#fetchAltitude(10,10, "alt.txt")
#alt2img("alt.txt","alt.png")
#img2csv("alt.png","data2.csv")
#appendAlt("data.csv", "data2.csv")
#black2medium("vegetation2.png", "vegetation3.png")
#veg2data("vegetation.png","data.csv")
#data2png("data.csv","tmp.png")
#all2csv("output.png","alt.png","vegetation.png", "data.csv")
#angle()
mergePics("tmp")




