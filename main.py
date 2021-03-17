import matplotlib.pyplot as plt

STLDebugx64 = []
CUSTOMDebugx64 = []
STLReleasex64 = []
CUSTOMReleasex64 = []
Iterations = []

file = open("BenchmarksDebugX64.txt", "r")
linesDebugx64 = file.readlines()
file.close()

file = open("BenchmarksReleaseX64.txt", "r")
linesReleasex64 = file.readlines()
file.close()

for line in linesDebugx64:
    positionOfColon = line.find(':') + 2
    if line.find("Amount of push_back:") != -1:
        Iterations.append(int(line[positionOfColon:]))
    elif line.find("STL") != -1:
        STLDebugx64.append(float(line[positionOfColon:]))
    elif line.find("CUSTOM") != -1:
        CUSTOMDebugx64.append(float(line[positionOfColon:]))

for line in linesReleasex64:
    positionOfColon = line.find(':') + 2
    if line.find("STL") != -1:
        STLReleasex64.append(float(line[positionOfColon:]))
    elif line.find("CUSTOM") != -1:
        CUSTOMReleasex64.append(float(line[positionOfColon:]))

for i in range(2, int(len(STLDebugx64)) + 1):
    Iterations.append(50 * i)

fig, ax = plt.subplots()  # Create a figure and an axes.
ax.plot(Iterations, STLDebugx64, label='STL in Debug x64')
ax.plot(Iterations, CUSTOMDebugx64, label='CUSTOM in Debug x64')

ax.set_xlabel('amount of push_backs')  # Add a x-label to the axes.
ax.set_ylabel('time in nano seconds')  # Add a y-label to the axes.
ax.legend()  # Add a legend.
ax.set_title("STL vs CustomContainer in Debug x64")
plt.savefig('STLvsCUSTOMDebugx64.png')

fig, ax = plt.subplots()
ax.plot(Iterations, STLReleasex64, label='STL in Release x64')
ax.plot(Iterations, CUSTOMReleasex64, label='CUSTOM in Release x64')
ax.set_xlabel('amount of push_backs')  # Add a x-label to the axes.
ax.set_ylabel('time in nano seconds')  # Add a y-label to the axes.
ax.legend()  # Add a legend.
ax.set_title("STL vs CustomContainer in Release x64")
plt.savefig('STLvsCUSTOMReleasex64.png')