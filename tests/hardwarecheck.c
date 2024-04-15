//
// Created by marin on 15/04/2024.
//

#include <stdio.h>
#include <strings.h>


unsigned gpioHardwareRevision(void)
{
   static unsigned rev = 0;

   FILE * filp;
   char buf[512];
   char term;

   if (rev) return rev;

   filp = fopen ("./proc/cpuinfo", "r");


   if (filp != NULL)
   {
      while (fgets(buf, sizeof(buf), filp) != NULL)
      {
         if (!strncasecmp("revision\t:", buf, 10))
         {
            if (sscanf(buf+10, "%x%c", &rev, &term) == 2)
            {
               printf("----> rev = %x\n", rev);
               if (term != '\n') rev = 0;
            }
         }
      }
      fclose(filp);
   }
   printf("----> term = %c\n", term);

   printf("----> rev = %x\n", rev);



   return rev;
}
int main() {
   printf("Hardware revision: %x\n", gpioHardwareRevision());
   return 0;
}


/* ----------------------------------------------------------------------- */
