import random
file=open('test.obj')
line = ""
color=[]
while True:
   line = file.readline()
   if line =="":
      break
   if line[0] == 'v':
      if(line[1]!=' '):
         continue
      line = line[2:len(line)-1]
      for i in range(0,len(line)):
         if line[i] == " ":
            line = line[0:i]+","+line[i+1:len(line)]
      color=[random.randint(0,1),random.randint(0,1),random.randint(0,1),1]
      print("{ XMFLOAT3("+line+"), XMFLOAT4("+str(color[0])+", "+str(color[1])+", "+str(color[2])+", 1.0f) },")
   if line[0] == 'f':
      f=[0,0,0]
      index = 0
      for i in range(1,len(line)):
         if line[i] == ' ':
            for j in range(1,7):
               if line[i+j] != "/":
                  f[index]*=10
                  f[index]+=int(line[i+j])
               else:
                  index+=1
                  break
      for i in range(2,-1,-1):
         print(str(f[i]-1)+",",end='')
      print("")
file.close()