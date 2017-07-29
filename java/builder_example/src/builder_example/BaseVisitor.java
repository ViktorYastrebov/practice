package builder_example;

import builder_example.Circle;
import builder_example.Triangle;
import builder_example.Rectangle;

public interface BaseVisitor {
  public void Visit(Circle c);
  public void Visit(Triangle t);
  public void Visit(Rectangle r);
}
