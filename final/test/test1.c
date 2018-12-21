/*
  test code version 3.0
*/

void main() {
  // [test] declaration statement.
  int times = 4, index = 0, step, _b = 4;
  // [test] for statement.
  for (step = 1, index = 0; index < 4;) {
    index = index + step;
    //[test] priority of operations
    3 + (4 - 5) * 4;
  }
  // [test] if else ambiguty and compound statement.
  if (_b < 0) {}
  if (_b > 0) {}
  else {}
}
