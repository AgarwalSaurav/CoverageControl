/** This file contains the declaration of the class TrainGNN using Torch C++ API.
 * The class TrainGNN takes local maps, communication maps, and obstacles maps as input, and
 * predicts the voronoi coverage features.
 *
 **/

#ifndef COVERAGECONTROL_TRAIN_GNN_H_
#define COVERAGECONTROL_TRAIN_GNN_H_


#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <thread>
#include <limits>
#include <yaml-cpp/yaml.h>
#include <torch/torch.h>

#include "gnn_module.h"

using namespace torch::indexing;
namespace F = torch::nn::functional;

namespace CoverageControlTorch {

	class TrainGNN {
		private:
			torch::Tensor train_maps_, val_maps_;
			torch::Tensor train_features_, val_features_;
			torch::Tensor train_actions_, val_actions_;
			torch::Tensor features_mean_, features_std_;
			torch::Tensor actions_mean_, actions_std_;
			torch::Tensor edge_weights_;
			torch::Device device_ = torch::kCPU;
			YAML::Node config_;
			YAML::Node cnn_config_;
			std::string data_dir_;
			size_t batch_size_ = 64;
			size_t num_epochs_ = 10;
			float learning_rate_ = 0.001;
			float weight_decay_ = 0.0001;
			int image_size_ = 32;

			/* std::shared_ptr<torch::optim::Adam> optimizer_; */
			std::shared_ptr<torch::optim::SGD> optimizer_;

		public:

			TrainGNN(std::string const &config_file) {
				if (torch::cuda::is_available()) {
					device_ = torch::kCUDA;
					std::cout << "Using CUDA" << std::endl;
				}
				LoadConfigs(config_file);
			}

			/** Train GNN model.
			 * @param dataset_dir: the directory of the dataset.
			 * @param num_layers: the number of convolutional layers.
			 * @param num_epochs: the number of epochs.
			 * @param learning_rate: the learning rate.
			 * @param batch_size: the batch size.
			 **/
			void Train() {

				LoadDataset();

				CoverageControlGNN model(config_);


				model->to(device_);

				optimizer_ = std::make_shared<torch::optim::SGD>(
						model->parameters(),
						torch::optim::SGDOptions(learning_rate_).weight_decay(weight_decay_).momentum(0.1));

				// Best model parameters
				float best_val_loss = std::numeric_limits<float>::max();
				std::vector<float> best_params;

				size_t dataset_size = train_maps_.size(0);
				for (size_t epoch = 1; epoch < num_epochs_ + 1; ++epoch) {
					optimizer_->zero_grad();
					for (size_t i = 0; i < dataset_size; i += batch_size_) {
						auto loss = TrainOneBatch(model, i);
						std::cout << "Epoch: " << epoch << ", Batch: " << i << ", Loss: " << loss << std::endl;
					}
					// Validate
					/* val_maps_ = val_maps_.to(device_); */
					/* auto pred = model->forward(val_maps_).to(torch::kCPU); */
					/* val_actions_ = val_actions_.to(torch::kCPU); */
					/* // Compute loss individually for each feature in features */
					/* /1* auto loss_vec = torch::norm(pred - val_actions_, 2, 0).to(torch::kCPU); *1/ */
					/* auto loss_vec = torch::mse_loss(pred, val_actions_, torch::Reduction::None).mean({0}).to(torch::kCPU); */
					/* std::cout << "Loss vector: " << loss_vec << std::endl; */
					/* auto actual_pred = pred * actions_std_ + actions_mean_; */
					/* auto actual_actions_ = val_actions_ * actions_std_ + actions_mean_; */
					/* auto accuracy = 100 * (torch::abs(actual_pred - actual_actions_).mean({0}))/(actual_actions_.mean({0})); */
					/* std::cout << "Accuracy: " << accuracy << std::endl; */
					/* auto loss = torch::mse_loss(pred, val_actions_); */
					/* std::cout << "Val loss: " << loss << std::endl; */
					/* std::cout << "Best Val loss: " << best_val_loss << std::endl; */
					/* if (loss.item<float>() < best_val_loss) { */
					/* 	best_val_loss = loss.item<float>(); */
					/* 	torch::save(model, data_dir_ + "/model.pt"); */
					/* 	torch::save(*optimizer_, data_dir_ + "/optimizer.pt"); */
					/* } */
				}
				std::cout << "Best validation loss: " << best_val_loss << std::endl;
			}

			float TrainOneBatch(CoverageControlGNN &model, size_t batch_idx) {
				torch::Tensor batch = train_maps_.index({Slice(batch_idx, batch_idx + batch_size_)});
				torch::Tensor batch_edge_weights = edge_weights_.index({Slice(batch_idx, batch_idx + batch_size_)});
				batch = batch.to(device_);
				batch_edge_weights = batch_edge_weights.squeeze(0);
				batch_edge_weights = batch_edge_weights.to_sparse();
				batch_edge_weights = batch_edge_weights.to(device_);
				auto x = model->forward(batch, batch_edge_weights);

				// Backward and optimize
				torch::Tensor batch_actions = train_actions_.index({Slice(batch_idx, batch_idx + batch_size_)}).to(device_);
				auto loss = torch::mse_loss(x, batch_actions);
				loss.backward();
				optimizer_->step();

				return loss.item<float>();

			}


			/** Function to load the dataset from the dataset directory.
			 * @param dataset_dir: the directory of the dataset.
			 **/
			void LoadDataset() {
				std::string local_maps_file = data_dir_ + "/local_maps.pt";
				std::string comm_maps_file = data_dir_ + "/comm_maps.pt";
				std::string obstacle_maps_file = data_dir_ + "/obstacle_maps.pt";
				std::string features_file = data_dir_ + "/normalized_coverage_features.pt";

				torch::Tensor local_maps;
				torch::load(local_maps, local_maps_file);
				int dataset_size = local_maps.size(0);
				local_maps = local_maps.unsqueeze(2).view({-1, 1, image_size_, image_size_});
				torch::Tensor comm_maps;
				torch::load(comm_maps, comm_maps_file);
				comm_maps = comm_maps.to_dense().view({-1, 2, image_size_, image_size_});
				torch::Tensor obstacle_maps;
				torch::load(obstacle_maps, obstacle_maps_file);
				obstacle_maps = obstacle_maps.to_dense().unsqueeze(2).view({-1, 1, image_size_, image_size_});
				torch::Tensor maps = torch::cat({local_maps, comm_maps, obstacle_maps}, 1);
				maps = maps.view({dataset_size, -1, 4, image_size_, image_size_});

				torch::Tensor features;
				torch::load(features, features_file);
				features = features.view({-1, features.size(2)});
				int output_dim = config_["CNN"]["OutputDim"].as<int>();
				features = features.index({Slice(), Slice(0, output_dim)});

				torch::Tensor actions;
				torch::load(actions, data_dir_ + "/actions.pt");

				// Split into train and val
				size_t num_train = 0.998 * maps.size(0);
				size_t num_val = maps.size(0) - num_train;
				train_maps_ = maps.index({Slice(0, num_train), Slice()});
				train_features_ = features.index({Slice(0, num_train), Slice()});
				train_actions_ = actions.index({Slice(0, num_train), Slice()});
				val_maps_ = maps.index({Slice(num_train, maps.size(0))});
				val_features_ = features.index({Slice(num_train, maps.size(0))});
				val_actions_ = actions.index({Slice(num_train, maps.size(0))});
				
				torch::load(features_mean_, data_dir_ + "/coverage_features_mean.pt");
				torch::load(features_std_, data_dir_ + "/coverage_features_std.pt");
				torch::load(actions_mean_, data_dir_ + "/actions_mean.pt");
				torch::load(actions_std_, data_dir_ + "/actions_std.pt");

				std::cout << "Loading edge weights" << std::endl;

				torch::load(edge_weights_, data_dir_ + "/edge_weights.pt");
				edge_weights_ = edge_weights_.to_dense();

				std::cout << "maps shape: " << maps.sizes() << std::endl;

			}

			void LoadConfigs(std::string const &config_file) {
				std::cout << "Using config file: " << config_file << std::endl;
				// Check if config_file exists
				if(not std::filesystem::exists(config_file)) {
					throw std::runtime_error("Could not open config file: " + config_file);
				}
				config_ = YAML::LoadFile(config_file);
				data_dir_ = config_["DataDir"].as<std::string>();
				batch_size_ = config_["GNNTraining"]["BatchSize"].as<size_t>();
				num_epochs_ = config_["GNNTraining"]["NumEpochs"].as<size_t>();
				learning_rate_ = config_["GNNTraining"]["LearningRate"].as<float>();
				weight_decay_ = config_["GNNTraining"]["WeightDecay"].as<float>();

				cnn_config_ = config_["CNN"]	;
				image_size_ = cnn_config_["ImageSize"].as<int>();
			}

	};

} // namespace CoverageControlTorch

#endif //COVERAGECONTROL_TRAIN_GNN_H_