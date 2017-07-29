package builder_example;

import builder_example.ColoredShape;

public interface BaseObserver {
  public abstract void Update(ColoredShape shape);
}
