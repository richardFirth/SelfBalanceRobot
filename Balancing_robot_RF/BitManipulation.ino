byte setBit(byte b, int pos) {
  if (pos < 0 || pos > 7)
    return b;
  b |= (1 << pos);
  return b;
}
 
byte clearBit(byte b, int pos) {
  if (pos < 0 || pos > 7)
    return b;
  b &= ~(1 << pos);
  return b;
}
 
boolean isSet(byte b, int pos) {
  if (pos < 0 || pos > 7)
    return false;
  return (b >> pos & 1) == 1;
}
 
boolean isClear(byte b, int pos) {
  return !isSet(b, pos);
}

byte customByte(boolean a, boolean b, boolean c, boolean d, boolean e, boolean f, boolean g, boolean h)
 {
   byte myByte = 0;
   if (a) myByte |= (1 << 7);
   if (b) myByte |= (1 << 6);
   if (c) myByte |= (1 << 5);
   if (d) myByte |= (1 << 4);
   if (e) myByte |= (1 << 3);
   if (f) myByte |= (1 << 2);
   if (g) myByte |= (1 << 1);
   if (h) myByte |= (1 << 0);
 
   return myByte;
 }

