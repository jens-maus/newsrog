# ifndef SWFINLINE
#  define SWFINLINE
# endif

static void SWFINLINE FastItoA(UBYTE *buff, ULONG v, ULONG td)
{
   static ULONG NumDiv[] = {
      10000000, 1000000, 10000, 1000, 100, 10, 1, 0 };

   static STRPTR NumStrs[] = {
      "00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
      "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
      "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
      "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
      "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
      "50", "51", "52", "53", "54", "55", "56", "57", "58", "59" };

   ULONG t, *y = NumDiv;

   if (v < 100) {
      if (v < 10 && !td) {
         *buff++ = v + '0';
         *buff   = '\0';
         return;
      }

      if (v < 60) {
         *buff++ = NumStrs[v][0];
         *buff++ = NumStrs[v][1];
         *buff   = '\0';
         return;
      }

      *buff++ = v / 10 + '0';
      *buff++ = v % 10 + '0';
      *buff   = '\0';
      return;
   }
   
   while (*y > v) y++;

   if (*y)
      while (*y) {
         t = v / *y;
         *buff++ = t + '0';
         v -= t * *y;
         y++;
      }
   else *buff++ = '0';

   *buff = '\0';
}
