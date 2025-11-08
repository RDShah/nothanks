import numpy as np
import os
from tqdm import tqdm
import torch
from torch import nn, from_numpy, relu, sigmoid
from torch.nn import Conv2d, Linear, Flatten, LeakyReLU, Module, MSELoss, L1Loss, Sequential

def model_predict_hands(model, hands):
    out = np.ones((len(hands), 24))
    x = np.ones((len(hands), 1, 33, 2), dtype=np.float32)
    for i, hand in enumerate(hands):
        x[i, 0] = hand2model_input(hand)
    out[:, :23] = model( from_numpy(x) ).squeeze(1).detach().numpy()
    return out

def hand2model_input(hand):
    x = np.unpackbits(np.array(hand, dtype=np.uint64).view(np.uint8), bitorder='little').reshape((2, 64)).T
    return x[3:36].reshape((33, 2))

def load_training_data(num_cards):
    num = 1
    x_raw = np.load(f'data/x_data_d{num_cards}_{num}.npy')
    y = np.load(f'data/y_data_d{num_cards}_{num}.npy')
    while 1:
        num += 1
        if not os.path.exists(f'data/x_data_d{num_cards}_{num}.npy'):
            break
        xx = np.load(f'data/x_data_d{num_cards}_{num}.npy')
        yy = np.load(f'data/y_data_d{num_cards}_{num}.npy')
        x_raw = np.concatenate((x_raw, xx))
        y = np.concatenate((y, yy))

    x = np.empty((x_raw.shape[0], 33, 2), dtype=np.float32)
    for i in range(x.shape[0]):
        x[i] = hand2model_input(x_raw[i])

    return from_numpy(x), from_numpy(y)

def load_validation_data(num_cards):
    x_raw = np.load(f'data/x_data_d{num_cards}_0.npy')
    y = np.load(f'data/y_data_d{num_cards}_0.npy')
    x = np.empty((x_raw.shape[0], 33, 2), dtype=np.float32)
    for i in range(x.shape[0]): x[i] = hand2model_input(x_raw[i])
    return from_numpy(x), from_numpy(y)

class MyModel(Module):
    def __init__(self):
        super(MyModel, self).__init__()
        num_filters = 8
        flat_size = 31 * num_filters

        self.a = Conv2d(1, num_filters, (3, 2))
        # self.a2 = Sigmoid()
        self.b = Flatten()
        self.c = Linear(flat_size, flat_size)
        self.c2 = LeakyReLU()
        self.d = Linear(flat_size, 32)
        self.d2 = LeakyReLU()
        self.e = Linear(32, 23)

    def forward(self, x):
        x = self.a(x)
        x = sigmoid(x)
        x = self.b(x)
        x = self.c(x)
        x = self.c2(x)
        x = self.d(x)
        x = self.d2(x)
        x = self.e(x)   
        x = sigmoid(x)     
        return x

Arches = {
    '23': MyModel,
    '22': MyModel,
    '21': MyModel,
    '20': MyModel,
    '19': MyModel,
    '18': MyModel,
    '17': MyModel,
    '16': MyModel,
    '15': MyModel,
    '14': MyModel,
    '13': MyModel,
    '12': MyModel,
    '11': MyModel,
    '10': MyModel,
    '9': MyModel,
    '8': MyModel,
    '7': MyModel,
    '6': MyModel,
    '5': MyModel,
    '4': MyModel,
    '3': MyModel,
    '2': MyModel,
    '1': MyModel,
}

def load_model(num_cards):
    model = Arches[str(num_cards)]()
    try:
        model.load_state_dict(torch.load(f'models/a{num_cards}.pt', weights_only=True))
    except:
        pass
    return model

def save_model(model, num_cards):
    torch.save(model.state_dict(), f'models/a{num_cards}.pt')

if __name__ == "__main__":
    num_cards = 21
    model = load_model(num_cards)
    
    criterion = L1Loss()
    optimizer = torch.optim.Adam(model.parameters(), lr=0.01)

    x, y = load_training_data(num_cards)
    xv, yv = load_validation_data(num_cards)

    if num_cards == 23:
        y = torch.round(y*10)/(10)
    elif num_cards == 22:
        y = torch.round(y*10*11)/(10*11)
    elif num_cards == 21:
        y = torch.round(y*10*11*12)/(10*11*12)
    elif num_cards == 20:
        y = torch.round(y*10*11*12*13)/(10*11*12*13)
    elif num_cards == 19:
        y = torch.round(y*10*11*12*13*14)/(10*11*12*13*14)
    elif num_cards == 18:
        y = torch.round(y*10*11*12*13*14*15)/(10*11*12*13*14*15)

    validation_loss = criterion(model(xv.unsqueeze(1)).squeeze(), yv).detach().numpy()

    for epoch in range(500):
        optimizer.zero_grad()
        loss = criterion(model(x.unsqueeze(1)).squeeze(), y)
        # loss = criterion(model(x), y)

        loss.backward()
        optimizer.step()

        validation_loss2 = criterion(model(xv.unsqueeze(1)).squeeze(), yv).detach().numpy()
        flag = False
        if validation_loss2 < validation_loss:
            validation_loss = validation_loss2
            save_model(model, num_cards)
            flag = True

        print("Epoch", epoch, '(test, validation) loss', loss.detach().numpy(), validation_loss2, "Saved!" if flag else "")
