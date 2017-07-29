package builder_example;

public class Circle extends ColoredShape {
    
private int m_radii;

  public Circle() {
      super();
      m_radii = 1;
  }

  public Circle(int radii) {
      super();
      m_radii = radii;
  }

  public void print_name() {
    System.out.println("I'm circle" + "radii = " + m_radii);
  }

  public ShapeType getID() {
    return Shape.ShapeType.CICLE;
  }
  
  public void Accept(BaseVisitor  v) {
    v.Visit(this);
  }
  
  public int getRadii() {
      return m_radii;
  }
  
}