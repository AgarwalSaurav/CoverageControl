import os
import sys
import copy
import torch
import torch_geometric

import CoverageControlTorch as cct
import CoverageControlTorch.data_loaders.data_loader_utils as dl_utils
from CoverageControlTorch.data_loaders.data_loaders import LocalMapGNNDataset
from CoverageControlTorch.models.gnn import CNNGNN
from CoverageControlTorch.trainers.trainer import TrainModel

# Set the device
device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")

config_file = sys.argv[1]
config = dl_utils.LoadToml(config_file)
dataset_path = config["DataDir"]
data_dir = dataset_path + "/data/"

gnn_model = config["GNNModel"]
model_dir = gnn_model["Dir"]
if not os.path.exists(model_dir):
    os.makedirs(model_dir)

model_file = model_dir + gnn_model["Model"]
optimizer_file = model_dir + gnn_model["Optimizer"]

training_config = config["GNNTraining"]
batch_size = training_config["BatchSize"]
num_epochs = training_config["NumEpochs"]
learning_rate = training_config["LearningRate"]
momentum = training_config["Momentum"]
weight_decay = training_config["WeightDecay"]

use_comm_map = config["GNN"]["UseCommMap"]

model = CNNGNN(config).to(device)

# cnn_pretrained_model = config["CNNModel"]["Dir"] + config["CNNModel"]["Model"]
# model.LoadCNNBackBone(cnn_pretrained_model)

train_dataset = LocalMapGNNDataset(data_dir, "train")
val_dataset = LocalMapGNNDataset(data_dir, "val")
test_dataset = LocalMapGNNDataset(data_dir, "test")

# for model in models:
model.register_buffer("actions_mean", train_dataset.targets_mean)
model.register_buffer("actions_std", train_dataset.targets_std)

print("Loaded datasets")
print("Train dataset size: {}".format(len(train_dataset)))

train_loader = torch_geometric.loader.DataLoader(train_dataset, batch_size=batch_size, shuffle=True, num_workers=24)
val_loader = torch_geometric.loader.DataLoader(val_dataset, batch_size=batch_size, shuffle=True, num_workers=24)
test_loader = torch_geometric.loader.DataLoader(test_dataset, batch_size=batch_size, shuffle=True, num_workers=24)

optimizer = torch.optim.Adam(model.parameters(), lr=learning_rate, weight_decay=weight_decay)

# Use mse loss for regression
criterion = torch.nn.MSELoss()

trainer = TrainModel(model, train_loader, val_loader, test_loader, optimizer, criterion, num_epochs, device, model_file, optimizer_file)

trainer.Train()

test_loss = trainer.Test()
print("Test loss: {}".format(test_loss))
