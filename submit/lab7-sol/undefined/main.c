int main(int argc, const char *argv[]) {
  for (int i = 1; i < argc; i++) {
    const char *name = argv[i];
    greet(name);
  }
}
