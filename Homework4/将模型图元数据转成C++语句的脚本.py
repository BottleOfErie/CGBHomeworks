import random
import re
file=open('summary.obj')
line = ""
color=[]

class vertex:
   def __init__(self,posstr):
      self.pos=posstr
      self.normal=[0.0,0.0,0.0]
      self.norm_cnt=0
   def addNorm(self,nrm):
      for i in range(3):
         self.normal[i]=self.normal[i]*self.norm_cnt+nrm[i]
         self.normal[i]=self.normal[i]/(self.norm_cnt+1)
      self.norm_cnt=self.norm_cnt+1
   def __str__(self):
      return f'{{ XMFLOAT3({self.pos}),XMFLOAT3({round(self.normal[0],6)},{round(self.normal[1],6)},{round(self.normal[2],6)}), XMFLOAT4({random.randint(0,1)}, {random.randint(0,1)}, {random.randint(0,1)}, 1.0f) }},'

vertices=[]
normals=[]
indices=[]

while True:
   line = file.readline()
   if line =="":
      break
   if line[0] == 'v':
      if line[1]=='n':
         strs=line.split(' ')
         normals.append([float(strs[1]),float(strs[2]),float(strs[3])])
         continue
      if(line[1]!=' '):
         continue
      line = line[2:len(line)-1]
      for i in range(0,len(line)):
         if line[i] == " ":
            line = line[0:i]+","+line[i+1:len(line)]
      vertices.append(vertex(line))
   if line[0] == 'f':
      line=line[2:len(line)]
      strs=re.split('[/ ]',line)
      f=[int(strs[0]),int(strs[3]),int(strs[6])]
      for i in range(3):
         vertices[f[i]-1].addNorm(normals[int(strs[3*i+2])-1])
      indices.append(f)
#      for i in range(2,-1,-1):
#         print(str(f[i]-1)+",",end='')
#      print("")
for vtx in vertices:
   print(str(vtx))
# for index in indices:
#    print(f'{index[0]},{index[1]},{index[2]},')
file.close()