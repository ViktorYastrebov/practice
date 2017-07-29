package builder_example;

import builder_example.BaseVisitor;

public class PrintVisitor implements BaseVisitor {
  public void Visit(Circle c) {
    System.out.println("Circle radii = " + c.getRadii());
  }
  public void Visit(Triangle t) {
    System.out.println("Triangle's sides are " + t.getLength1() + ", " + t.getLength2() + ", " + t.getLength3());
  }
  public void Visit(Rectangle r) {
    System.out.println("Rectangle length = " + r.getLenght() + ", width =" + r.getWidth());
  }
}
