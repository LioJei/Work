// HelloWorld.qml
import QtQuick 2.0
Rectangle {
    // name this element root
    id: root
    // properties: <name>: <value>
    width: 120; height: 240
    // color property
    color: "#D8D8D8"
    // Declare a nested element (child of root)
    Image {
        id: rocket
        // reference the parent
        x: (parent.width - width)/2; y: 40
        source: 'assets/rocket.png'
    }
    // Another child of root
    Text {
        // un-named element
        // reference element by id
        y: rocket.y + rocket.height + 20
        // reference root element
        width: root.width
        horizontalAlignment: Text.AlignHCenter
        text: 'Rocket'
    }
}