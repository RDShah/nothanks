import numpy as np
import matplotlib.pyplot as plt

def load(file, shape):
    with open(f'data/{file}.bin', 'rb') as f:
        return np.array([*f.read()], dtype=np.uint8).reshape(shape)

    # return np.load(f'data/{file}.npy')

deck_top = load('dt', (-1, 2))
one_in = load('oi', (-1, 2))
two_in = load('ti', (-1, 4))

one_totals = [
    # np.zeros((36), int),
    # np.zeros((36), int),
    [ [] for _ in range(36) ],
    [ [] for _ in range(36) ],
]

two_totals = [
    # np.zeros((36, 36), int),
    # np.zeros((36, 36), int),
    # np.zeros((36, 36), int),
    # np.zeros((36, 36), int)
    [ [[] for __ in range(36)] for _ in range(36) ],
    [ [[] for __ in range(36)] for _ in range(36) ],
    [ [[] for __ in range(36)] for _ in range(36) ],
    [ [[] for __ in range(36)] for _ in range(36) ],
]

for d, o, t in zip(deck_top, one_in, two_in):
    # one_counts[d[0]] += 1
    # two_counts[d[0], d[1]] += 1    
    for i in range(2):
        one_totals[i][d[0]] += [o[i]]

    for i in range(4):
        two_totals[i][d[0]][d[1]] += [t[i]]

fig, axs = plt.subplots(2, 2)

# for i in range(2):
#     data = one_totals[i]
#     # data = [np.array(x, dtype=int) for x in one_totals[i] ]
#     axs[0, i].plot([np.median(x) for x in data])

for i in range(4):
    data = two_totals[i]
    mat = [[np.median(y) for y in x] for x in data]
    axs[i // 2, i % 2].matshow(np.array(mat))

plt.show()
# plt.savefig('x.png')
quit()

# plt.plot(tot[0] / cnt)
# plt.plot(tot[1] / cnt)
# plt.show()

for i in range(2):
    plt.matshow(one_totals[i] / counts)
    plt.show()

for i in range(4):
    plt.matshow(two_totals[i] / counts)
    plt.show()

# for row in two_counts:
#     print(row)
# print()

# for a in two_freqs:
#     for row in a:
#         print(row)
#     print()