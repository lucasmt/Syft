import os
import commands
import sys
import random

maxlength = int(sys.argv[1])
ninputs = int(sys.argv[2])
noutputs = int(sys.argv[3])
num = 50
length = 1
realizable = [7, 8, 9, 10, 12, 13, 19, 20, 22, 24]
unrealizable = [1, 2, 3, 4, 5, 6, 11, 14, 15, 16, 17, 18, 21, 23]
basecases = realizable
prefix = "lilyreal/"
while length <= maxlength:
	n = 1
	while n <= num:
		formulas = []
		a = 1
		in_items = ["P"+str(i) for i in range(ninputs)]
		out_items = ["P"+str(ninputs+i) for i in range(noutputs)]
		#tmp = random.choice(basecases)
		# tmp = 16
		while a <= length: 
			tmp = random.choice(basecases)
			# print tmp
			f = open(prefix+'lilydemo'+str(tmp)+'.ltl')
			formula = f.readline().strip('\n')
			p = open(prefix+'lilydemo'+str(tmp)+'.part')
			formula = formula.upper();
			for line in p.readlines():
				if 'inputs' in line:
					line = line.strip('\n').strip(' ')
					line = line.split(" ")
					i = 1
					while i < len(line):
                                                P = random.choice(in_items)
						formula = formula.replace(line[i].upper(), P)
						i = i + 1
					# print line
					# print formula
				if 'outputs' in line:
					line = line.strip('\n').strip(' ')
					line = line.split(" ")
					i = 1
					# print line
					while i < len(line):
                                                P = random.choice(out_items)
						formula = formula.replace(line[i].upper(), P)
						i = i + 1

			# 		print formula
			# print formula
			formulas.append('(')
			formulas.append(formula)
			formulas.append(')')
			formulas.append(' & ')
			a = a + 1
		formulas.pop()

                newformula = " ".join(formulas)
                
		newpart = ".inputs "
		for item in in_items:
			newpart = newpart + item + ' '
		newpart = newpart.rstrip() + "\n.outputs "
		for item in out_items:
			newpart = newpart + item  + ' '
		newpart = newpart.rstrip()




		path = 'lilymulti/case_'+str(length)+'_'+str(num)
		isExists=os.path.exists(path)
		if not isExists:
			os.mkdir(path)
		ff = open(path+'/lily'+str(n)+'.ltl', 'w')
		ff.write(newformula)
		ff.close()
		fp = open(path+'/lily'+str(n)+'.part', 'w')
		fp.write(newpart)
		fp.close()
		n = n + 1
	length = length + 1

