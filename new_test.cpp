#include <iostream>
#include <string>
#include "osrng.h"

using namespace std;


#include "cryptlib.h"
#include "filters.h"
#include "files.h"
#include "modes.h"
#include "hex.h"
#include "des.h"
#include "secblock.h"
using namespace CryptoPP;

string find_binstring(byte a)
{

}
int find_hamming_distance(byte a,byte b)
{   int count=0;
   cout<<"lll";
  byte t= a^b;
  cout<<";;;;"<<endl;
  for(int i=0;i<8;i++)
  {
    count=count+(t&(1<<i));

  }
  return count;

}
int find_avalanche_effect(vector <byte> input,vector <byte> cipher)
{
  cout<<"in avalanche"<<endl;
  int count=0;
  for(int i=0;i<input.size();i++)
  {
    cout<<"heerreee"<<endl;
    count=count+find_hamming_distance(input[i],cipher[i]);
  }
  return count;
}


vector<byte> flip_bit(vector <byte> input,int n)
{
  int byte_no=n/8;
  int bit_pos=n%8;
  byte temp=input[7-byte_no];
  temp=temp^(1<<bit_pos);
  input[7-byte_no]=temp;
  return input;

}

int main(int argc, char* argv[])
{


  AutoSeededRandomPool prng;
  SecByteBlock key(DES::DEFAULT_KEYLENGTH);
  cout<<"key size="<<key.size()<<endl;
  prng.GenerateBlock(key, key.size());

  byte iv[DES::BLOCKSIZE];
  prng.GenerateBlock(iv, sizeof(iv));
  vector<byte> plain;
  HexEncoder encoder(new FileSink(cout));
  vector <byte> prev_cipher;

  string str("Attacks!");
  std::copy(str.begin(), str.end(), std::back_inserter(plain));

  for(int i=0;i<2;i++)
  {
    vector<byte> cipher,recover;
    cout << "Plain text: ";
  encoder.Put(plain.data(), plain.size());
  encoder.MessageEnd();
  cout << endl;

  /////////////////////////////////////////////////////////////

  ECB_Mode<DES>::Encryption enc;
  enc.SetKey(key,key.size());

  // Make room for padding
  cipher.resize(plain.size()+DES::BLOCKSIZE);
  ArraySink cs(&cipher[0],cipher.size());

  ArraySource(plain.data(), plain.size(), true,
    new StreamTransformationFilter(enc, new Redirector(cs)));

  // Set cipher text length now that its known
  cipher.resize(cs.TotalPutLength());

  cout << "Cipher text: ";
  encoder.Put(cipher.data(),cipher.size());
  encoder.MessageEnd();
  cout << endl;
  cout<<cipher.size()<<endl;
   cout<<plain.size()<<endl;

  /////////////////////////////////////////////////////////////

  ECB_Mode<DES>::Decryption dec;
  dec.SetKey(key,key.size());

  // Recovered text will be less than cipher text
  recover.resize(cipher.size());
  ArraySink rs(&recover[0], recover.size());

  ArraySource(cipher.data(), cipher.size(), true,
    new StreamTransformationFilter(dec, new Redirector(rs)));

  // Set recovered text length now that its known
  recover.resize(rs.TotalPutLength());

  cout << "Recovered text: ";
  encoder.Put(recover.data(), recover.size());
  encoder.MessageEnd();
  cout << endl;

  vector<byte> new_plain=flip_bit(plain,1);
  plain=new_plain;
  if(i==0)
    continue;
  else
  {
    int c;
    c=find_avalanche_effect(cipher,prev_cipher);
    cout<<"Avalanche effect is: "<<endl;
  }
  
  prev_cipher.resize(cipher.size());
  prev_cipher=cipher;




  }

  
  return 0;
}