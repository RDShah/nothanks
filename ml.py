import numpy as np
from tqdm import tqdm
import torch
from torch import nn, from_numpy, relu, sigmoid
from torch.nn import Conv2d, Linear, Flatten, LeakyReLU, Module, MSELoss, L1Loss

def hand2model_input(hand):
    x = np.unpackbits(np.array(hand, dtype=np.uint64).view(np.uint8), bitorder='little').reshape((2, 64)).T
    return x[3:36].reshape((33, 2))

def combine_dataset(num_cards, datasets):
    x_raw = np.empty((1000 * datasets, 2), dtype=np.uint64)
    print(x_raw.shape)
    y = np.empty((1000 * datasets, 23), dtype=np.float32)
    for dataset_number in range(1, datasets + 1):
        x_raw[1000*dataset_number - 1000 : 1000*dataset_number] = np.load(f'data/x_data_d{num_cards}_{dataset_number}.npy')
        y[1000*dataset_number - 1000 : 1000*dataset_number] = np.load(f'data/y_data_d{num_cards}_{dataset_number}.npy')

    x = np.empty((x_raw.shape[0], 33, 2), dtype=np.float32)
    for i in range(x.shape[0]):
        x[i] = hand2model_input(x_raw[i])

    return from_numpy(x), from_numpy(y)

class MyModel(Module):
    def __init__(self):
        super(MyModel, self).__init__()
        num_filters = 16
        flat_size = 31 * num_filters

        self.a = Conv2d(1, num_filters, (3, 2))
        self.a2 = LeakyReLU()
        self.b = Flatten()
        self.c = Linear(flat_size, flat_size)
        self.c2 = LeakyReLU()
        self.d = Linear(flat_size, 32)
        self.d2 = LeakyReLU()
        self.e = Linear(32, 23)

    def forward(self, x):
        x = self.a(x)
        x = self.a2(x)
        x = self.b(x)
        x = self.c(x)
        x = self.c2(x)
        x = self.d(x)
        x = self.d2(x)
        x = self.e(x)   
        x = sigmoid(x)     
        return x

model_path = 'evaluator_d20.pt'

model = MyModel()
model.load_state_dict(torch.load(model_path, weights_only=True))

criterion = L1Loss()
optimizer = torch.optim.Adam(model.parameters(), lr=0.001)

x, y = combine_dataset(20, 28)

for epoch in range(500):
    optimizer.zero_grad()
    loss = criterion(model(x.unsqueeze(1)).squeeze(), y)
    loss.backward()
    optimizer.step()

    print("Epoch", epoch, 'loss', loss.detach().numpy())

    torch.save(model.state_dict(), model_path)

out = model(x[0:1].unsqueeze(1))
print(out - y[0])
