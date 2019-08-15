import matplotlib.pyplot as plt
import os
y_fp = []
y_ap = []
x = [1,5,10,25,50,90]
with open('time_file1.txt') as f:
    l = list(f)
    for i in l:
        if i != "\n":
            if i[-1]=='\n':
                k = i[:-1]
            else:
                k = i 
            y_fp.append(int(k))
with open('time_file2.txt') as f2:
    l = list(f2)
    for i in l:
        if i != "\n":
            if i[-1]=='\n':
                k = i[:-1]
            else:
                k = i 
            y_ap.append(int(k))
if os.path.exists("time_file1.txt"):
  os.remove("time_file1.txt")
else:
  print("The file does not exist")
if os.path.exists("time_file2.txt"):
  os.remove("time_file2.txt")
else:
  print("The file does not exist")
    # y_ap.append(int(f2.readline()))
plt.plot(x,y_fp,'r-')
plt.plot(x,y_ap,'b-')
# plt.hlines(y_fp,0,x, linestyle="dashed")
# plt.hlines(y_ap,0,x ,linestyle="dashed")
# plt.hlines(y, 0, x, linestyle="dashed")
plt.ylabel('Time axis')
plt.xlabel('Support%')
plt.show()