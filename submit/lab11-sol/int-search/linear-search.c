/** Return index of element in a[nElements]; < 0 if not found. */
int
search_for_element(int a[], int nElements, int element)
{
  int index = -1;
  for (int i = 0; i <= nElements; i++){
    if (a[i] == element)
      index = i;
  }
  return index;
}
