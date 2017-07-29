package builder_example;

public class Rectangle extends ColoredShape {
    
private int m_length, m_width;

  public Rectangle() {
      super();
      m_length = 1;
      m_width  = 1;
  }
  
  public Rectangle(int length, int width) {
      super();
      m_length = length;
      m_width = width;
  }

  public void print_name() {
    System.out.println("I'm rectangle" + "length = " + m_length + ", width = " + m_width);
  }

  public ShapeType getID() {
    return Shape.ShapeType.RECTANGLE;
  }

  public void Accept(BaseVisitor  v) {
      v.Visit(this);
  }
  
  public int getLenght() {
      return m_length;
  }
  
  public int getWidth() {
      return m_width;
  }
  
}
