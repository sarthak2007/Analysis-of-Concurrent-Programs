import numpy as np
import matplotlib.pyplot as plt

N = 5
ind = np.arange(N)  # the x locations for the groups
width = 0.25       # the width of the bars

fig = plt.figure()
ax = fig.add_subplot(111)

N = [8.82, 6.56, 22.46, 29.49, 14.57]
FT2 = [40.97, 17.45, 33.81, 185.08, 30.82]
FT2S50 = [32.17, 21.48, 50.66, 153.50, 51.20]
FT2S10 = [26.06, 18.73, 51.76, 142.44, 45.56]

FT2 = [x / y for x, y in zip(FT2, N)]
rects1 = ax.bar(ind, FT2, width, color='r')

FT2S50 = [x / y for x, y in zip(FT2S50, N)]
rects2 = ax.bar(ind+width, FT2S50, width, color='g')

FT2S10 = [x / y for x, y in zip(FT2S10, N)]
rects3 = ax.bar(ind+width*2, FT2S10, width, color='b')

ax.set_ylabel('Normalized execution time w.r.t the empty N tool')
ax.set_xticks(ind+width)
ax.set_xticklabels( ('avrora', 'luindex', 'lusearch', 'sunflow', 'xalan') )
ax.legend( (rects1[0], rects2[0], rects3[0]), ('FT2', 'FT2S 50%', 'FT2S 10%') )

def autolabel(rects):
    for rect in rects:
        h = rect.get_height()
        ax.text(rect.get_x()+rect.get_width()/2., 1.05*h, '%d'%int(h),
                ha='center', va='bottom')

# autolabel(rects1)
# autolabel(rects2)
# autolabel(rects3)

# plt.show()
plt.savefig('bar_graph.png')