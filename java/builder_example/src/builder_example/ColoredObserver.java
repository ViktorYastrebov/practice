package builder_example;

import builder_example.BaseObserver;

public class ColoredObserver implements BaseObserver {
   public void Update(ColoredShape shape) {
      System.out.println("color has been changed. Obj ID = " + shape.getID());
   }
}
