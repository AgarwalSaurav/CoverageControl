#ifndef SHAREDBUFFER_H
#define SHAREDBUFFER_H

constexpr int NUM_IPS = 20; // Number of IP addresses
constexpr int BUFFER_SIZE = 1024; // Adjust the buffer size as needed

extern char sharedBuffer[NUM_IPS][BUFFER_SIZE];

#endif // SHAREDBUFFER_H