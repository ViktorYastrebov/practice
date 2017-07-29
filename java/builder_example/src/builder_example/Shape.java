package builder_example;

import builder_example.BaseVisitor;
import builder_example.BaseObserver;

public interface Shape {
    
  public enum ShapeType {
    TRIANGLE,
    CICLE,
    RECTANGLE
  }

  public enum Color {
    RED,
    BLUE,
    GREEN,
    WHITE,
    ORANGE
  }
  
  public void print_name();
  public void setColor(Color color);
  public ShapeType getID();
  
  public void Accept(BaseVisitor  v);
  public void Attach(BaseObserver v);
  public void Detach(BaseObserver v);
}
