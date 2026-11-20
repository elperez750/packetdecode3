#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int main(int argc, char* argv[]) {
        FILE* fp;
        int byte1 = 0;
        int byte2 = 0;
        int counter = 0;
        int ret = 0;
        int ihl = 0;

        // If the number of arguments is less than 2, then we know that there was no filename given
        if (argc < 2)
        {
            printf("Error: no filename given\n");
            ret = 1;
        }


        else
        {

            fp = fopen(argv[1], "rb");

            // This means that our file pointer is not
            if (fp == NULL)
            {
                printf("Error: could not open %s\n", argv[1]);
                ret = 2;
            }
            else {
                printf("Ethernet header:\n");
                printf("---------------\n");


                printf("Destination MAC address:         ");
                for (int i = 0; i < 5; i++) {
                    byte1 = fgetc(fp);
                    printf("%02X:", byte1);
                }

                byte1 = fgetc(fp);
                printf("%02X\n", byte1);


                printf("Source MAC address:              ");
                for (int i = 0; i < 5; i++) {
                    byte1 = fgetc(fp);
                    printf("%02X:", byte1);
                }


                // IP specific code
                byte1 = fgetc(fp);
                printf("%02X\n", byte1);
                printf("Type:                            ");


                byte1 = fgetc(fp);
                // Shifting the first byte by 8. This would get us 0000 1000 0000 0000
                byte1 = byte1 << 8;

                // Get the next byte, which is 0000 if we are dealing with ipv4
                byte2 = fgetc(fp);
                // This will OR what we have with the next bit. so we have the following:
                // We will only perform the OR on the last 8 bits. This essentially combines whatever the lower byte was with what we currently have with type.
                /*
                 * 0000 1000 0000 0000
                 *           0000 0000
                 *
                 */
                 byte2 = byte2 | byte1;

                printf("%04X\n", byte2);


                printf("\n");
                // Place to add IP header specific code

                if (byte2 == 0x0800) {
                    printf("IPv4 Header:\n");
                    printf("---------------\n");
                    printf("Version:                         ");
                    byte1 = fgetc(fp);
                    printf("%02X\n", byte1 >> 4);


                    printf("Internet header length:          ");
                    ihl = byte1 & 0x0F;
                    printf("%02X\n", ihl);

                    // DSCP
                    printf("DSCP:                            ");
                    byte1 = fgetc(fp);
                    printf("%02X\n", (byte1 >> 2) & 0x3F);


                    // ECN
                    printf("ECN:                             ");
                    byte1 = byte1 & 0x3F;
                    printf("%02X      ", (byte1));
                    if (byte1 == 0) {
                        printf("Non-ECT Packet\n");
                    } else if (byte1 == 1 || byte1 == 2) {
                        printf("ECN-Capable Packet\n");
                    } else {
                        printf("Congestion Experienced\n");
                    }

                    // Total Length
                    printf("Total Length:                    ");
                    // Read the first (most significant) byte and shift it to the upper 8 bits
                    byte1 = fgetc(fp);
                    byte1 = (byte1 & 0xFF) << 8;
                    // Read the second (least significant) byte and combine it into the lower 8 bits
                    byte2 = fgetc(fp);
                    byte2 = byte2 | (byte1);
                    // Print the combined 16-bit decimal value
                    printf("%u\n", byte2);

                    // Identification
                    printf("Identification:                  ");
                    byte1 = fgetc(fp);
                    byte1  = byte1 << 8;
                    byte2 = fgetc(fp);
                    byte2 = byte2 | byte1;
                    printf("%d\n", byte2);

                    // Flags
                    printf("Flags:                           ");
                    byte1 = fgetc(fp);
                    // This will check to see if we have a one in the rsv place. If we have a 1 there, then ANDing with 1 will give us 1
                    if ((byte1 >> 5 & (0x01)) == 1) {
                        printf("More Flags \n");
                    }
                    // The same logic here, except we shift the "don't fragment" bit all the way to the right. We will check if this is one.
                    if ((byte1 >> 6 & (0x01)) == 1) {
                        printf("Don't Fragment\n");
                    }
                    else {
                        printf("No Flags Set\n");
                    }

                    // Fragment offset
                    printf("Fragment offset:                 ");
                    byte1 = byte1 << 8;
                    byte2 = fgetc(fp);
                    byte2 = byte2 | byte1;
                    byte2 = byte2 & 0x1FFF;
                    printf("%d\n", byte2);


                    // Time to live
                    printf("Time to live:                    ");
                    byte1 = fgetc(fp);
                    printf("%d\n", byte1);
                    printf("Protocol:                        ");
                    byte1 = fgetc(fp);
                    printf("%d\n", byte1);


                    printf("Header checksum:                 ");
                    byte1 = fgetc(fp);
                    byte1 = byte1 << 8;
                    byte2 = fgetc(fp);
                    byte2 = byte2 | byte1;
                    printf("0x%04X\n", byte2);


                    printf("Source IP address:               ");
                    for (int i = 0; i < 3; i++) {
                        byte1 = fgetc(fp);
                        printf("%d.", byte1);
                    }
                    byte1 = fgetc(fp);
                    printf("%d\n", byte1);


                    printf("Destination IP address:          ");
                    for (int i = 0; i < 3; i++) {
                        byte1 = fgetc(fp);
                        printf("%d.", byte1);
                    }
                    byte1 = fgetc(fp);
                    printf("%d\n", byte1);



                    if (ihl - 5 <= 0) {
                        printf("Options:       No Options\n");
                    }
                    else {
                        for (int i = 0; i < ihl-5; i++) {
                            byte1 = fgetc(fp);
                            byte2 = fgetc(fp);
                            printf("IP Option word #%d:               0x%02X%02X", i, byte1, byte2);

                            byte1 = fgetc(fp);
                            byte2 = fgetc(fp);
                            printf("%02X%02X\n", byte1, byte2);


                        }
                    }


                }


                // Payload code
                printf("\nPayload:\n");

                while ((byte1 = fgetc(fp)) != EOF) {
                    counter++;
                    printf("%02X", byte1);


                    if (counter % 32 == 0) {
                        printf("\n");
                    }

                    else if (counter % 8 == 0) {
                        printf("      ");
                    }

                    else {
                        printf(" ");
                    }
                }

                fclose(fp);
            }


            }

        return ret;
    }
