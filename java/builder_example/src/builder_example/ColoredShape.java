package builder_example;

import java.util.LinkedList;

import builder_example.Shape;

public abstract class ColoredShape implements Shape {
    
  public ColoredShape() {
      super();
      m_Color = Color.WHITE;
      observers_ = new LinkedList<BaseObserver>();
  }
  
  public abstract void print_name();
  public abstract ShapeType getID();
  
  public void setColor(Color color) {
     m_Color = color;
     Notify();
  }
  abstract public void Accept(BaseVisitor  v);
  
  public void Attach(BaseObserver v) {
      observers_.add(v);
  }
  
  public void Detach(BaseObserver v) {
    observers_.remove(v);
  }
  
  public void Notify() {
      for(BaseObserver obj: observers_) {
          obj.Update(this);
      }
  }

  private Color m_Color;
  LinkedList<BaseObserver> observers_;
}