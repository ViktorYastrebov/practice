package builder_example;

import java.util.HashMap;

public class ShapeBuilder {

  private HashMap<Shape.ShapeType, Shape> map; 
  public ShapeBuilder()
  {
    map = new HashMap<Shape.ShapeType, Shape>();
    map.put(Shape.ShapeType.TRIANGLE, new Triangle());
    map.put(Shape.ShapeType.CICLE, new Circle());
    map.put(Shape.ShapeType.RECTANGLE, new Rectangle());
  }
  
  public Shape getById(Shape.ShapeType type) {
    return map.get(type);
  }
}
