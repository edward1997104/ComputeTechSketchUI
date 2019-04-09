

from plyfile import PlyData
import numpy as np
import sys

def read_stress_level(file_name):
	RATIO = 100
	result = []
	current_min = np.finfo(np.float32).max
	current_max = np.finfo(np.float32).min
	with open(file_name, "r") as file:
		cnt = 0
		for line in file:
			if cnt == 0:
				cnt = cnt + 1
				continue
			input = line.split()
			input = [float(x) for x in input[1:]]
			for i in range(3):
				input[i] = input[i] * RATIO
			result.append(input)

			if input[3] > current_max:
				current_max = input[3]
			if input[3] < current_min:
				current_min = input[3]

	reult = np.array(result)
	print ("min:" + str(current_min))
	print ("max:" + str(current_max))
	print ("result:")
	print (result)

	with open("checking_object.obj", "w") as file:
		for result_data in result:
			file.write("v "+str(result_data[0]) +" " + str(result_data[1]) + " " + str(result_data[2])+"\n")
	return current_min, current_max, result

def main():

	print("reading data......")
	plydata = PlyData.read("plane_modified.ply")
	print("finish reading data.")

	min_stress, max_stress, stress_array = read_stress_level("plane_result.txt")

	mid_stress = (min_stress + max_stress) / 2

	print("mid-stress: " + str(mid_stress))
	print("starting color the vertices......")

	for i in range(len(plydata.elements[0].data)):

		if i % 10000 == 0:
			print (str(i) + " vertices completed!")

		current_min_dis = np.finfo(np.float32).max
		current_min_index = -1
		for j in range(len(stress_array)):

			dis = abs(plydata.elements[0].data[i][0] - stress_array[j][0])
			+ abs(plydata.elements[0].data[i][1] - stress_array[j][1])
			+ abs(plydata.elements[0].data[i][2] - stress_array[j][2])

			if dis < current_min_dis:
				current_min_index = j
				current_min_dis = dis

		smapled_stress = stress_array[current_min_index][3]

		color_map = [
		[0, 0, 255],
		[0, 128, 255],
		[0, 255, 255],
		[120, 240, 180],
		
		]
#
#		diff_of_stress = smapled_stress - mid_stress
#		diff_of_stress = abs(diff_of_stress)
#		color_index = int((diff_of_stress / mid_stress) * 255)
#		if smapled_stress < mid_stress:
#			plydata.elements[0].data[i][3] = 0
#			plydata.elements[0].data[i][4] = 255 - color_index
#			plydata.elements[0].data[i][5] = color_index
#		else:
#			plydata.elements[0].data[i][5] = 0
#			plydata.elements[0].data[i][4] = 255 - color_index
#			plydata.elements[0].data[i][3] = color_index

		if i % 1000 == 0:
			print (str(smapled_stress) + " " + str(current_min_index))
			print ("color index: " + str(plydata.elements[0].data[i][3]) + " "
			+ str(plydata.elements[0].data[i][4]) + " "
			+ str(plydata.elements[0].data[i][5]) + "\n" )

	print("writing the output file....")
	PlyData(plydata.elements, text = True).write("my_trial_2.ply")
	print("finish coloring as" + " my_trial_2.ply" +"!")

if __name__ == "__main__":
	main()
