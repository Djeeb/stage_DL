#include "SGD.hpp"
#include "function_gen.hpp"
using namespace std;
ofstream file("../../data/mnist_SVRG/SGD_0_025_expdecay_0_001.dat");

int main(){
	
	int batch = 10;

	//______________________________________________Initializing dataset
	auto train_set = torch::data::make_data_loader(
                     torch::data::datasets::MNIST("../../data").map(
                     torch::data::transforms::Stack<>()),10);

	//_______________________________________Initializing neural network
	int d = 784;
	int n = 60000; 
	int n_test = 10000;
	int epochs = 100;
	int k = 0;
	double learning_rate = 0.03;
	double decay = 0.01;
	double cost;
	nnet neuralnet(n,batch,d,100,10,learning_rate,"CPU");
	torch::optim::SGD optimizer(neuralnet.parameters(), 0.01);	
				
	//_________________________________________________Running algorithm
	cout << "Iter" << "\t\t" << "loss" << endl;
	auto t1 = chrono::system_clock::now();

	for(int i=1; i <= epochs;i++){
		k=1;
		for(auto& sample : *(train_set)){
			optimizer.zero_grad();
			
			
			auto X = sample.data.reshape({batch,d}).to(options_double);		
			auto Y = at::one_hot(sample.target,10).to(options_double);
			
			X = neuralnet.forward( X );
			auto loss =  neuralnet.mse_loss( X , Y );
			loss.backward();
			neuralnet.update_SGD();
		

			k++;
		}
		cost = neuralnet.compute_cost() * batch;
		cout << "" << i << "\t" << cost << endl;
		file << "" << i << "\t" << cost << endl;
		neuralnet.learning_rate = learning_rate/(1+decay*i);
	}
	
	auto t2 = chrono::system_clock::now();
	chrono::duration<double> diff = t2 - t1;
	cout << "Phase d'apprentissage terminée en " << diff.count() << " sec" << endl;

	//______________________________________________________Evaluating model
	auto test_set = torch::data::make_data_loader(
                     torch::data::datasets::MNIST("../../data",torch::data::datasets::MNIST::Mode::kTest).map(
                     torch::data::transforms::Stack<>()),n_test);
                     
	cout << "\n* Précision sur le test set : ";
	file << "\n# Précision sur le test set : ";
	for(auto& sample : *test_set){
			auto X_test = sample.data.reshape({n_test,d}).to(options_double);
			auto Y_test = sample.target.to(options_int);
			double error = error_rate(Y_test,neuralnet.predict(X_test));
			cout << error << endl;
			file << error << endl;
		}
}
