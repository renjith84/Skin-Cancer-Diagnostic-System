using namespace std;
#include<iostream>
#include<fstream>
#include<time.h>
#include<math.h>
#include<stdlib.h>
#include<iomanip>
#include "preprocessing.cpp"

#define LEARNING_RATE  .33
#define TRAIN_FILE     "image.dat"
#define DESIRED_OUTPUT "desired.dat"
#define WEIGHT_FILE    "weight.dat"
#define INPUT_FILE     "tst_img.dat"
#define TRUE           0
#define FALSE          1

time_t t;

class Layer
{
public:
        Layer(){}
        ~Layer(){}
        virtual void randomizeWeight(){}
        virtual void loadWeight(){}     
        virtual double squashFunct(double net_val){}
        virtual void writeOutput(int neuron_no,double ouput){}
        virtual void writeLocalGradient(int neuron_no,double error_giradient){}
        virtual double readOutput(int neuron_no){} 
        virtual int getLayerSize(){}
        virtual double getWeight(int neuron_no,int input_no){}
        virtual void putWeight(int neuron_no,int input_no,double new_weight){};
        virtual double readError(Layer& layer,int current_index){}
        virtual double readLocalGradient(int index){}

};

class InputLayer:public Layer
{
        int layer_size;
        double *input;
        long pos;
public:
        InputLayer(){}
        ~InputLayer(){}
        void initialize(int no_neurons);
        double readOutput(int neuron_no)        {       return input[neuron_no];}
        int getLayerSize()      {       return layer_size;      };
        void readInput(char *file_name);
        void showInput();
        void resetFilePointer(){ pos=0;}
        
};

void InputLayer::initialize(int no_neurons)
{
        layer_size=no_neurons;
        input=new double[layer_size];
        pos=0;
}

void InputLayer::readInput(char *file_name)
{       
        FILE *fp=fopen(file_name,"r");
        fseek(fp,pos,SEEK_SET);
        for(int i=0;i<layer_size;i++)
        {
                fscanf(fp,"%lf ",&input[i]);
                input[i]=input[i]/100;
        }
        if(!feof(fp))
                pos=ftell(fp);
        else
        {
                pos=0;
                //cout<<"end of input file"<<endl;
        }
}

void InputLayer::showInput()
{
        for(int i=0;i<layer_size;i++)
                cout<<input[i]<<"  ";
        cout<<endl;
}

class HiddenLayer:public Layer
{
       int layer_size;
       int prev_layer_size;
       double **weight;
       double *output;
       double *error_gradient;
public:
        HiddenLayer(){}
        ~HiddenLayer(){}
       void randomizeWeight();
       double squashFunct(double net_val);
       double readError(Layer& layer,int neuron_index);
       void showLocalGradient();
       void showOutput();
       void initialize(int no_neurons,Layer& PrevLayer );
       double readOutput(int neuron_no){        return output[neuron_no]; }
       int getLayerSize()       { return layer_size;}
       double getWeight(int neuron_no,int input){return weight[neuron_no][input];       }
       void putWeight(int neuron_no,int input_no,double new_weight){ weight[neuron_no][input_no]=new_weight;}
       void writeOutput(int neuron_no,double output1){  output[neuron_no]=output1;}
       void writeLocalGradient(int neuron_no,double eg) {error_gradient[neuron_no]=eg;}
       double readLocalGradient(int index){return error_gradient[index];}
       long loadWeight(long pos);
        void showWeight();
};      
void HiddenLayer::showWeight()
{
        // Nothing;
}
                
void HiddenLayer::initialize(int no_neurons,Layer& PrevLayer)
{
        layer_size=no_neurons;
        prev_layer_size=PrevLayer.getLayerSize();
        output=new double[layer_size];
        error_gradient=new double[layer_size];
        weight=new double*[layer_size];
        for(int i=0;i<layer_size;i++)
                weight[i]=new double[prev_layer_size];
}

void HiddenLayer::randomizeWeight()
{
        double random_weight=0;
        for(int i=0; i<layer_size;i++)
        for(int j=0;j<prev_layer_size;j++)
        {
                random_weight=rand()%100;
                weight[i][j]=(2*(random_weight/100))-1;
        }
        
}

long HiddenLayer::loadWeight(long file_pos)
{
        FILE *fp=fopen(WEIGHT_FILE,"r");
        fseek(fp,file_pos,SEEK_SET);
        for(int i=0;i<layer_size;i++)
        for(int j=0;j<prev_layer_size;j++)
                fscanf(fp,"%lf ",&weight[i][j]);
        return ftell(fp);
}
        

double HiddenLayer::squashFunct(double net_val)
{
        double output=1/(1+exp(-1*net_val));
        return output;
}

double HiddenLayer::readError(Layer& layer,int neuron_index)
{
        double error=0;
        for(int prev_layer_index=0;prev_layer_index<layer.getLayerSize();prev_layer_index++)
        {
                error+=(layer.getWeight(prev_layer_index,neuron_index)*layer.readLocalGradient(prev_layer_index));
        }
        return error;
}

void HiddenLayer::showLocalGradient()
{
        cout<<"\n";
        for(int i=0;i<layer_size;i++)   cout<<error_gradient[i]<<"  ";
}

void HiddenLayer::showOutput()
{       
        cout<<"\n";
        for(int i=0;i<layer_size;i++)   cout<<output[i]<<"  ";
}

class OutputLayer:public Layer
{
        int layer_size;
        int prev_layer_size;
        double *desired_output,*output;
        double **weight;
        double *error_gradient;
        int pos;
        time_t t;//because of repeting same values for other layers
public:
        OutputLayer(){}
        ~OutputLayer(){}
        void initialize(int no_neurons,Layer& PrevLayer);
        void randomizeWeight();
        double squashFunct(double net_val);
        void showLocalGradient();
        void showOutput();
        double getOutput(){return output[0];}
        int getLayerSize(){     return layer_size;      }
        double readOutput(int neuron_no){       return output[neuron_no];}
        double getWeight(int neuron_no,int input){return weight[neuron_no][input];      }
        void putWeight(int neuron_no,int input_no,double new_weight){ weight[neuron_no][input_no]=new_weight;}
        void writeOutput(int neuron_no,double output1){ output[neuron_no]=output1;      }
        double readError(Layer& layer,int index)        {return (desired_output[index]-output[index]);  }
        void writeLocalGradient(int neuron_no,double eg)        {error_gradient[neuron_no]=eg;  }
        double readLocalGradient(int neuron_no) {return error_gradient[neuron_no];}
        long loadWeight(long pos);
        void readDesiredOutput();
        void showDesiredOutput();
        void showWeight();
};

void OutputLayer::initialize(int no_neurons,Layer& PrevLayer)
{
        layer_size=no_neurons;
        prev_layer_size=PrevLayer.getLayerSize();
        desired_output=new double[layer_size];
        output=new double[layer_size];
        error_gradient=new double[layer_size];
        weight=new double*[layer_size];
        pos=0;
        for(int i=0;i<layer_size;i++)
                weight[i]=new double[prev_layer_size];
        desired_output[0]=1;desired_output[1]=1,desired_output[2]=1,desired_output[3]=0;

}

void OutputLayer::randomizeWeight()
{
        double random_weight=0;
        for(int i=0; i<layer_size;i++)
        for(int j=0;j<prev_layer_size;j++)
        {
                random_weight=rand()%100;
                weight[i][j]=(2*(random_weight/100))-1;
                                                                      
        }       
}

long OutputLayer::loadWeight(long file_pos)
{
        FILE *fp=fopen(WEIGHT_FILE,"r");
        fseek(fp,file_pos,SEEK_SET);
        for(int i=0;i<layer_size;i++)
        for(int j=0;j<prev_layer_size;j++)
                fscanf(fp,"%lf ",&weight[i][j]);
        return ftell(fp);
}

void OutputLayer::readDesiredOutput()
{
        FILE *fp=fopen(DESIRED_OUTPUT,"r");
        fseek(fp,pos,SEEK_SET);
        for(int i=0;i<layer_size;i++)
                fscanf(fp,"%lf ",&desired_output[i]);
        if(!feof(fp)) 
                pos=ftell(fp);
        else    
        {
                pos=0;
                //cout<<"end of desired file"<<endl;
        }
}

void OutputLayer::showDesiredOutput()
{
        for(int i=0;i<layer_size;i++)
                cout<<desired_output[i]<<endl;
}

double OutputLayer::squashFunct(double net_val)
{
        double output=1/(1+exp(-1*net_val));
        return output;
}

void OutputLayer::showOutput()
{
        cout<<"\n";     
        for(int i=0;i<layer_size;i++)   cout<<output[i]<<"  ";
}


void OutputLayer::showLocalGradient()
{
        cout<<"\n";
        for(int i=0;i<layer_size;i++)   cout<<error_gradient[i]<<"  ";
}

void OutputLayer::showWeight()
{
        cout<<weight[0][0];
}       

class Neuron
{       
        double net_val;
        int neuron_no;
        double learning_rate;
        double output;
public:
        Neuron(){}
        ~Neuron(){}
        void start(int index)   {neuron_no=index;net_val=0;output=0;}
        void calculateOutput(Layer &previous_layer,Layer &current_layer);
        void calculateError(Layer &current_layer,Layer &processed_layer,int index);
        void adjust_weight(Layer &current_layer,Layer &input_to_layer,int neuron_index);        
};

void Neuron::calculateOutput(Layer &previous_layer,Layer &current_layer)
{
        net_val=0;

        for(int i=0;i<previous_layer.getLayerSize();i++)
        {
                net_val=net_val+(previous_layer.readOutput(i)*current_layer.getWeight(neuron_no,i));
        }
        output=current_layer.squashFunct(net_val);
        current_layer.writeOutput(neuron_no,output);
}
        
void Neuron::calculateError(Layer &current_layer,Layer &processed_layer,int index)
{
        double sigmoidal_derivative=0;
        double error_gradient=0;
        double neuron_output=current_layer.readOutput(index);
        double neuron_error=current_layer.readError(processed_layer,index);
        sigmoidal_derivative=neuron_output*(1-neuron_output);
        error_gradient=neuron_error*sigmoidal_derivative;
        current_layer.writeLocalGradient(index,error_gradient);
}

void Neuron::adjust_weight(Layer &current_layer,Layer &input_to_layer,int neuron_index)
{
        double delta_weight=0;
        double prev_weight=0;
        for(int j=0;j<input_to_layer.getLayerSize();j++)
        {
                delta_weight=LEARNING_RATE*current_layer.readLocalGradient(neuron_index)*input_to_layer.readOutput(j);
                prev_weight=current_layer.getWeight(neuron_index,j);
                current_layer.putWeight(neuron_index,j,prev_weight+delta_weight);       
        }
}


class Network
{
        void forwardPass(Layer &CurrentLayer,Layer &PrevLayer);
        void reversePass(Layer &InputLayer,Layer& MiddleLayer,Layer &OutputLayer);
        void updateWeight(Layer &CurrentLayer,Layer &PrevLayer);
        InputLayer input_layer;
        HiddenLayer hidden1,hidden2;
        OutputLayer out_layer;
        int file_append;
public:
        Network(int ip_layerSize,int hd1_layerSize,int hd2_layerSize,int out_layerSize){
                input_layer.initialize(ip_layerSize);
                hidden1.initialize(hd1_layerSize,input_layer);
                hidden2.initialize(hd2_layerSize,hidden1);
                out_layer.initialize(out_layerSize,hidden2);
                srand((unsigned)time(&t));
        }
        ~Network(){cout<<"Quiting ..."<<endl;}

        void train();
        void test();
        void randomizeWeights();
        void message(char*);
        void delay();
};
        void Network::delay()
        {
                for(int i=0;i<=1;i++);
        }

        void Network::message(char *str)
        {            
                cout<<setw(40)<<left<<str<<"   [OK] "<<"\n";
                delay();
        }

void Network::test()
{
        long pos=0;
        getchar();
        message("Reading preprocessed image");
        input_layer.readInput(INPUT_FILE);
        message("Processing input layer");
        message("Loading weights");
        pos=hidden1.loadWeight(pos);
        pos=hidden2.loadWeight(pos);
        out_layer.loadWeight(pos);
        message("Processing hidden layers");
        forwardPass(hidden1,input_layer);
        forwardPass(hidden2,hidden1);
        forwardPass(out_layer,hidden2);
        message("Firing output neurons");
        double fired_output=out_layer.getOutput();
        cout<<"Fired output             :   "<<fired_output<<endl;
        cout<<endl<<"TEST RESULT                :   ";
        if(fired_output>0.5)
                cout<<"[ YES ]";
        else
                cout<<"[ NO ]";
        getchar();      
}

void Network::train()
{
        int i=0;
        long int pos=0;
        pos=hidden1.loadWeight(pos);
        pos=hidden2.loadWeight(pos);
        out_layer.loadWeight(pos);
        message("Loading weights");
        getchar();      
        while(i<250)
        {       
                message("Reading new image");
                input_layer.readInput(TRAIN_FILE);
                message("Processing input layer");
                out_layer.readDesiredOutput();
                message("Reading desired output");
                message("Processing hidden layers");
                forwardPass(hidden1,input_layer);
                forwardPass(hidden2,hidden1);
                forwardPass(out_layer,hidden2);
                message("Firing output layer neurons");
                //out_layer.showOutput();
                double fired_output=out_layer.getOutput();
                cout<<"Fired output                 :   ";
                delay();
                //if(fired_output>0.5)  
                        cout<<fired_output;
                //else
                reversePass(hidden2,out_layer,hidden2);
                reversePass(hidden1,hidden2,out_layer);
                reversePass(input_layer,hidden1,hidden2);
                message("Calculating local gradients");
                message("Adjusting weights");
                i++;
        }
        cout<<"Wait... Updating weight files."<<endl;
        file_append=FALSE;
        updateWeight(hidden1,input_layer);
        updateWeight(hidden2,hidden1);
        updateWeight(out_layer,hidden2);
        file_append=FALSE;
}
        
void Network::forwardPass(Layer &CurrentLayer,Layer &PrevLayer)
{
        int layer_size=CurrentLayer.getLayerSize();
        Neuron neuron[layer_size];
        for(int i=0;i<layer_size;i++)
        {       
                neuron[i].start(i);
                neuron[i].calculateOutput(PrevLayer,CurrentLayer);
        }
}

void Network::reversePass(Layer& InputLayer,Layer& MiddleLayer,Layer& OutputLayer)
{
        int layer_size=MiddleLayer.getLayerSize();
        Neuron neuron[layer_size];
        cout<<endl;
        for(int i=0;i<layer_size;i++)
        {
                neuron[i].calculateError(MiddleLayer,OutputLayer,i);
                neuron[i].adjust_weight(MiddleLayer,InputLayer,i);
                
        }
}

void Network::updateWeight(Layer& CurrentLayer,Layer& PrevLayer)
{
        ofstream out;
        if(file_append==TRUE)
                out.open(WEIGHT_FILE,ios::app);
        else
                out.open(WEIGHT_FILE,ios::out);
        int curr_layer_size=CurrentLayer.getLayerSize();
        int prev_layer_size=PrevLayer.getLayerSize();
        for(int i=0;i<curr_layer_size;i++)
        {
                for(int j=0;j<prev_layer_size;j++)
                        out<<CurrentLayer.getWeight(i,j)<<" ";
                out<<endl;
        }
        out<<endl;
        out.close();
        file_append=TRUE;
}

void Network::randomizeWeights()
{
        hidden1.randomizeWeight();
        hidden2.randomizeWeight();
        out_layer.randomizeWeight();
        file_append=FALSE;
        updateWeight(hidden1,input_layer);
        updateWeight(hidden2,hidden1);
        updateWeight(out_layer,hidden2);
        file_append=FALSE;
}

int main()
{
        Network network(65536,128,15,1);
        preprocessing pp;
        int opt=0;
        do
        {   

                cout<<"\n\n\t\t\t ============================ \n";
                cout<<"\t\t\t SKIN CANCER DIAGNOSIS SYSTEM \n";
                cout<<"\t\t\t ============================ \n";
                cout<<"\n\t\t\t     1) Randomize Weights"<<endl;
                cout<<"\n\t\t\t     2) Train Network"<<endl;
                cout<<"\n\t\t\t     3) Test Network"<<endl;
                cout<<"\n\t\t\t     4) Exit"<<endl;
                cout<<"\n\t\t\t ============================ \n";
                cout<<"\n\t\t\t     Enter your choice : ";
                cin>>opt;
                cout<<endl;
                switch(opt)
                {
                        case 1:
                              cout<<endl<<"Wait ... Randomizing weights."<<endl;
                              network.randomizeWeights();       
                              break;
                        case 2:
                              pp.main1(0);
                              cout<<endl<<"Wait ... Training starts."<<endl;
                              network.train();
                              break;
                        case 3:
                              pp.main1(1);
                              network.test();
                              break;
                        case 4:
                              exit(0);
                }
        }while(1);
}

