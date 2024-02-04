import math
import pandas as pd
import matplotlib.pyplot as plt

class_ = ['insert', 'delete', 'search']

fig, ax1 = plt.subplots()
plt.title('IP lookups')
plt.xlabel('clock cycle')
ax2 = ax1.twinx()
ax1.set_ylabel('insert and delete freq', color='tab:blue')
ax1.tick_params(axis='y', labelcolor='tab:blue')
for k in range(2):
	df = pd.read_csv(class_[k])
	df_min = df['0'].min()
	df_max = df['0'].max()

	step = math.ceil(df_max/50)
	for i in range(0, df_max, step):
		df['0'] = df['0'].apply(lambda x: i if (i+step)>x and x>=i else x)
	df_freq = df['0'].value_counts()
	df_freq.sort_index(inplace=True)
	print(df_freq)
	ax1.plot(df_freq.index, df_freq, marker="o")

ax2.set_ylabel('search freq', color='black')
ax2.tick_params(axis='y', labelcolor='black')
for k in range(2,3,1):
	df = pd.read_csv(class_[k])
	df_min = df['0'].min()
	df_max = df['0'].max()
	df['0'] = df['0'].apply(lambda x: 100000 if x>100000 else x)

	step = math.ceil(100000/50)
	for i in range(0, df_max, step):
		df['0'] = df['0'].apply(lambda x: i if (i+step)>x and x>=i else x)
	df_freq = df['0'].value_counts()
	df_freq.sort_index(inplace=True)
	print(df_freq)
	ax2.plot(df_freq.index, df_freq, color='green', marker="o")

fig.tight_layout()
plt.show()