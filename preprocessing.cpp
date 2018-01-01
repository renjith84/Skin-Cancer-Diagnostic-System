using namespace std;
#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<string.h>
#include<unistd.h>
#include<iostream>
#include<math.h>
#include<fstream>
#include<stdlib.h>
#include<iomanip>
#define FILE_DEST    "image.dat"
char FILE_SOURCE[100];
int a=0;

// Class for preprocessing the image
class preprocessing
{
        private:
                char buffer[64],temp[64];
                int inhandle,outhandle,bytes;
                float** image_s,*image_d[200];
                int appnd;
        public:
                preprocessing()
                {
                        image_s=new float*[256];
                        for(int i=0;i<256;i++)
                        image_s[i]=new float[256];
                        appnd=0;
                }
                int userMenu(int);
                int printStatus(char*);
                int readImage(int);
                int checkBMP(void);
                int checkbit(void);
                void message(char*);
                //The red 3byes are converted to the crresponding decimal values and are stored in
                //Float variables. This reppresents the values of a pixel
                float convertFloat(char*);
                void delay();
                void append(){appnd=1;};
};

int preprocessing::checkBMP()
 {
    int s[20],inhandle;
    inhandle=open(FILE_SOURCE,O_RDONLY);

        if(inhandle==NULL)
        {
                return(2);
        }
        else
        {
                bytes=read(inhandle,buffer,2);
                if(buffer[0]=='B'&& buffer[1]=='M')
                        return(1);//cout<<"\n The file is BMP";
                else
                        return(0);//cout<<"\n The file is not BMP";
        }
}

int preprocessing::checkbit()
{
        int s[20],inhandle;
        int header=0;
        float bit;
        inhandle=open(FILE_SOURCE,O_RDONLY);
        while(header<14)
        {
                bytes=read(inhandle,buffer,2);
                header++;
        }
        bytes=read(inhandle,buffer,2);
        bit=convertFloat(buffer);
        if(bit==24)
                return(3);
        else
                return(4);
}

void preprocessing::delay()
{
        for(int i=0;i<=10000;i++);
}

void preprocessing::message(char*str)
{
        cout<<setw(40)<<left<<str<<"   [OK]"<<"\n";
        delay();
}

int preprocessing::printStatus(char*string)
{
        outhandle=open("status.txt",O_CREAT|O_WRONLY);
        write(outhandle,string,strlen(string));
        close(outhandle);
        return(0);
}

//The Image preprocessing goes here
int preprocessing::readImage(int mode)
{
        if(checkBMP()==2)
        {
                cout<<"Such a file does not exist\n";
                exit(0);

        }
        else if(checkBMP()==0)
        {
                cout<<"Not a valid BMP file\n";
                exit(0);

        }
        if(checkbit()==3)
        {
                cout<<"The image is 24 bit\n";
                return(0);
        }
        else
        {
                //cout<<"The image is not 24 bit\n";
        }

        message("\nReading source image");
        ofstream fout;
        inhandle=open(FILE_SOURCE,O_RDONLY);
        if(mode==0)
        {
                if(appnd==0)
                {
                        fout.open(FILE_DEST,ios::out);
                        //cout<<"OUT\n";
                }
                else
                {
                        fout.open(FILE_DEST,ios::app);
                        //cout<<"APP\n";
                }
        }
        else
        {
                fout.open("tst_img.dat",ios::out);
        }
        int a=0;
        float  max;
        int header=0;
        double f=0;
        long int x=0,y=0;
        message("Preprocessing image source header");
        message("Preprocessing image source data");
        while(1)
        {
                bytes=read(inhandle,buffer,3);
                if(header<=18)
                {
                        header++;
                        //cout<<convertFloat(buffer)<<"  ";
                        continue;
                }
                //exit(0);
                if(bytes>0)
                {
                        image_s[x][y]=convertFloat(buffer);
                        y++;

                        if(y==256)
                        {
                                x++;
                                y=0;
                        }
                        if(x==256)
                        {
                                break;
                        }
                }
                else
                {
                        break;
                }
        }
        close(inhandle);
        max=1.24385e+07;
        double img;
        message("Normalising the image");
        message("Converting to gray scale");
        message("Writing image to temporary file");
        for(x=0;x<256;x++)
        {
        //cout<<x<<"\n";
                for(y=0;y<256;y++)
                {
                        img=(double)((image_s[x][y]+5)/(max*2));
                        fout<<img<<" ";
                }
        }
        return(0);
}

float preprocessing::convertFloat(char* buffer)
{
        int i,k,andmask;
        int temp[24];
        int t=24;
        float img=0;
        for(int j=0;j<3;j++)

        {
                for(i=7;i>=0;i--)
                {
                        --t;
                        andmask==1<<i;
                        k=*(buffer+j)&andmask;
                        if(k==0)
                        {
                                temp[t]=0;
                        }
                        else
                        {
                                temp[t]=1;
                        }
                }
        }
        for(i=0;i<24;i++)
        {
                img=img+(temp[i]*pow(2,i));
        }
        return(img);
}
int preprocessing::userMenu(int mode)
  {
        int no,i,inhandle;
        double value;
        FILE* dop=fopen("desired.dat","w+");
        inhandle=open(FILE_SOURCE,O_RDONLY);
        char a[20][25];
        if(mode==0)
        {
                cout<<"Enter the no: of files   :  ";
                cin>>no;
                cout<<"Enter the file name(s)\n";
        for(i=0;i<no;i++)
        {
                cout<<"File "<<i+1<<"                   :  ";
                cin>>a[i];
                cout<<"Deried Value             :  ";
                cin>>value;
                fprintf(dop,"%lf ",value);
                if(inhandle==NULL) continue;
        }
                fclose(dop);
        preprocessing pr;

        for(i=0;i<no;i++)
        {
                strcpy(FILE_SOURCE,a[i]);
                pr.readImage(0);
                pr.append();
        }
        }
        else
        {
                cout<<"Enter the file name      :  ";
                cin>>FILE_SOURCE;
                preprocessing pr;
                pr.readImage(1);
        }
        //getchar();
        return(0);
}
