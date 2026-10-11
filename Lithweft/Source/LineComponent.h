//
//  ==============================================================================
//
//    LineComponent.h
//    Created: 11 Nov 2023 2:55:37pm
//    Author:  刘昊
//
//  ==============================================================================

//
//class LineComponent : public juce::Component {
//public:
//    LineComponent(PortComponent* startPort, PortComponent* endPort, juce::Component* relativeToComponent)
//           : startPort(startPort), endPort(endPort), relativeToComponent(relativeToComponent) {
//           if (startPort && endPort) {
//               auto bounds = calculateLineBoundsRelativeTo(relativeToComponent);
//               setBounds(bounds);
//           }
//           setInterceptsMouseClicks(false, true); // Enable mouse events only for children
//       }
//
////       void paint(juce::Graphics& g) override {
////           if (!startPort || !endPort) return;
////
////           juce::Point<int> startPoint = startPort->getCentreRelativeTo(this);
////           juce::Point<int> endPoint = endPort->getCentreRelativeTo(this);
////
////           // Calculate control points for the curve
////           juce::Point<float> controlPoint1(startPoint.getX(), (startPoint.getY() + endPoint.getY()) / 2);
////           juce::Point<float> controlPoint2(endPoint.getX(), (startPoint.getY() + endPoint.getY()) / 2);
////
////           // Create a Path to draw the curve
////           juce::Path curvePath;
////           curvePath.startNewSubPath(startPoint.getX(), startPoint.getY());
////           curvePath.cubicTo(controlPoint1.getX(), controlPoint1.getY(),
////                             controlPoint2.getX(), controlPoint2.getY(),
////                             endPoint.getX(), endPoint.getY());
////
////           g.setColour(isSelected ? juce::Colours::blue : juce::Colours::red);
////           g.strokePath(curvePath, juce::PathStrokeType(2.0f));
////
////           if (isSelected) {
////               g.setColour(juce::Colours::yellow);
////               g.fillEllipse(startPoint.getX() - 3, startPoint.getY() - 3, 6, 6);
////               g.fillEllipse(endPoint.getX() - 3, endPoint.getY() - 3, 6, 6);
////           }
////       }
//    
//    void paint(juce::Graphics& g) override
//    {
//        if (!startPort || !endPort) return;
//
//        juce::Point<int> startPoint = startPort->getCentreRelativeTo(this);
//        juce::Point<int> endPoint = endPort->getCentreRelativeTo(this);
//
//        // Calculate control points for the curve
//        juce::Point<float> controlPoint1(startPoint.getX(), (startPoint.getY() + endPoint.getY()) / 2);
//        juce::Point<float> controlPoint2(endPoint.getX(), (startPoint.getY() + endPoint.getY()) / 2);
//
//        // Create a Path to draw the curve
//        juce::Path curvePath;
//        curvePath.startNewSubPath(startPoint.getX(), startPoint.getY());
//        curvePath.cubicTo(controlPoint1.getX(), controlPoint1.getY(),
//                          controlPoint2.getX(), controlPoint2.getY(),
//                          endPoint.getX(), endPoint.getY());
//
//        // Draw the curve
//        g.setColour(isSelected ? juce::Colours::blue : juce::Colours::red);
//        g.strokePath(curvePath, juce::PathStrokeType(2.0f));
//
//        // Draw the selectable sensitive area (rectangular area around the curve)
//        juce::Rectangle<float> selectionRect = curvePath.getBounds().expanded(5.0f); // Expand for visual margin
//        g.setColour(juce::Colours::yellow.withAlpha(0.5f)); // Semi-transparent yellow for visibility
//        g.drawRect(selectionRect, 1.0f); // Draw rectangle
//
//        if (isSelected) {
//            g.setColour(juce::Colours::yellow);
//            g.fillEllipse(startPoint.getX() - 3, startPoint.getY() - 3, 6, 6);
//            g.fillEllipse(endPoint.getX() - 3, endPoint.getY() - 3, 6, 6);
//        }
//    }
//
//
//       bool hitTest(int x, int y) override {
//           juce::Point<float> point(static_cast<float>(x), static_cast<float>(y));
//           return distanceFromPointToLine(point) < 5.0f; // 5.0f is the tolerance
//       }
//
//    
//   
//    // Add 'const' to the method signature
//    bool isPointNearSegment(const juce::Point<float>& point, const juce::Line<float>& segment, float tolerance) const {
//        // Calculate the projection of the point onto the line segment
//        float lineLengthSquared = segment.getLengthSquared();
//        juce::Point<float> vectorA = point - segment.getStart();
//        juce::Point<float> vectorB = segment.getEnd() - segment.getStart();
//
//        // Calculate the dot product manually
//        float dotProduct = vectorA.x * vectorB.x + vectorA.y * vectorB.y;
//        float t = std::max(0.0f, std::min(1.0f, dotProduct / lineLengthSquared));
//        juce::Point<float> projection = segment.getStart() + t * (segment.getEnd() - segment.getStart());
//
//        // Calculate the distance from the point to the projection
//        return point.getDistanceFrom(projection) < tolerance;
//    }
//
//    bool isPointNearLine(const juce::Point<int>& point, int tolerance = 5) const {
//        juce::Point<float> startPoint = startPort->getCentreRelativeTo(this).toFloat();
//        juce::Point<float> endPoint = endPort->getCentreRelativeTo(this).toFloat();
//
//        // Calculate control points for the curve
//        juce::Point<float> controlPoint1(startPoint.getX(), (startPoint.getY() + endPoint.getY()) / 2);
//        juce::Point<float> controlPoint2(endPoint.getX(), (startPoint.getY() + endPoint.getY()) / 2);
//
//        // Create a Path for the curved line
//        juce::Path curvePath;
//        curvePath.startNewSubPath(startPoint);
//        curvePath.cubicTo(controlPoint1, controlPoint2, endPoint);
//
//        // Flatten the path into line segments and check each segment for proximity to the given point
//        juce::AffineTransform transform; // Identity transform
//        juce::PathFlatteningIterator iterator(curvePath, transform);
//        while (iterator.next()) {
//            // Calculate the distance from the point to the current line segment
//            juce::Line<float> segment(juce::Point<float>(iterator.x1, iterator.y1), juce::Point<float>(iterator.x2, iterator.y2));
//            if (isPointNearSegment(point.toFloat(), segment, tolerance)) {
//                return true; // The point is near the curve
//            }
//        }
//
//        return false; // The point is not near the curve
//    }
//    
//    
//    float distanceFromPointToLine(const juce::Point<float>& point) const {
//        // Line segment start and end points
//        juce::Point<float> start = startPort->getCentreRelativeTo(this).toFloat();
//        juce::Point<float> end = endPort->getCentreRelativeTo(this).toFloat();
//
//        // Calculate the distance using the formula
//        float A = point.getX() - start.getX();
//        float B = point.getY() - start.getY();
//        float C = end.getX() - start.getX();
//        float D = end.getY() - start.getY();
//
//        float dot = A * C + B * D;
//        float len_sq = C * C + D * D;
//        float param = -1;
//        if (len_sq != 0) //in case of 0 length line
//            param = dot / len_sq;
//
//        float xx, yy;
//
//        if (param < 0) {
//            xx = start.getX();
//            yy = start.getY();
//        } else if (param > 1) {
//            xx = end.getX();
//            yy = end.getY();
//        } else {
//            xx = start.getX() + param * C;
//            yy = start.getY() + param * D;
//        }
//
//        float dx = point.getX() - xx;
//        float dy = point.getY() - yy;
//        return std::sqrt(dx * dx + dy * dy);
//    }
//
//
//    juce::Rectangle<int> calculateLineBoundsRelativeTo(juce::Component* relativeToComponent) {
//        auto startPoint = startPort->getCentreRelativeTo(relativeToComponent);
//        auto endPoint = endPort->getCentreRelativeTo(relativeToComponent);
//
//        int x = std::min(startPoint.getX(), endPoint.getX());
//        int y = std::min(startPoint.getY(), endPoint.getY());
//        int width = std::abs(startPoint.getX() - endPoint.getX());
//        int height = std::abs(startPoint.getY() - endPoint.getY());
//
//        return juce::Rectangle<int>(x, y, width, height).expanded(5);
//    }
//    
//    
//    void setSelected(bool isSelected) {
//        this->isSelected = isSelected;
//        repaint();
//    }
//
//    bool getSelected() const {
//        return isSelected;
//    }
//
//private:
//    PortComponent* startPort;
//    PortComponent* endPort;
//    juce::Component* relativeToComponent;
//    bool isSelected = false;
//
//
//
//};
#pragma once

#include <JuceHeader.h>

class LineComponent : public juce::Component {
public:
    LineComponent(PortComponent* startPort, PortComponent* endPort, juce::Component* relativeToComponent)
        : startPort(startPort), endPort(endPort), relativeToComponent(relativeToComponent) {
            if (startPort && endPort) {
                auto bounds = calculateLineBoundsRelativeTo(relativeToComponent);
                setBounds(bounds);
            }
            setInterceptsMouseClicks(false, true); // Enable mouse events only for children
    }
    
    void paint(juce::Graphics& g) override {
        if (!startPort || !endPort) return;

        // Update the curve path and hit test areas
        createCurvePath();

        // Check if component bounds need to be updated
        auto bounds = calculateLineBoundsRelativeTo(relativeToComponent);
        if (getBounds() != bounds) {
            setBounds(bounds);
        }


        // Draw the hit test areas for debugging
//        g.setColour(juce::Colours::yellow);
//        for (const auto& area : hitTestAreas) {
//            g.drawRect(area, 1.0f); // Draw each hit test area
//        }
        
        // Draw the curve
        g.setColour(isSelected ? juce::Colours::blue : juce::Colours::black);
        g.strokePath(curvePath, juce::PathStrokeType(2.0f));

        // Optionally draw hit test areas for debugging
        // ...
    }



    bool hitTest(int x, int y) override {

        return false;
    }

    const std::vector<juce::Rectangle<float>>& getHitTestAreas() const {
        return hitTestAreas;
    }
    
    
    
    // Method to check if a given point is over the line
    bool isPointOverLine(const juce::Point<int>& point) const {
//        std::cout<<"isUnderThelineComponent "<<"x: "<<point.x<<" y:"<<point.y<<std::endl;
//        juce::Point<float> clickPoint(static_cast<float>(x), static_cast<float>(y));
        for (const auto& area : hitTestAreas) {
//            std::cout<<"area bound:"<<area.toString()<<std::endl;
            if (area.contains(point.toFloat())) {
                std::cout<<"in the area:"<<std::endl;
                return true;
            }
        }
        return false;
    }
    

    
    void setSelected(bool isSelected) {
        this->isSelected = isSelected;
        repaint();
    }

    bool getSelected() const {
        return isSelected;
    }
    
    PortComponent* getStartPort() const
    {
        return startPort;
    }
    
    PortComponent* getEndPort() const{
        return endPort;
    }

private:
    PortComponent* startPort;
    PortComponent* endPort;
    juce::Component* relativeToComponent;
    bool isSelected = false;
    std::vector<juce::Rectangle<float>> hitTestAreas;
    juce::Path curvePath;  // Member variable for storing the line path

    void createCurvePath() {
        curvePath.clear(); // Clear the existing path

           juce::Point<float> startPoint = startPort->getCentreRelativeTo(this).toFloat();
           juce::Point<float> endPoint = endPort->getCentreRelativeTo(this).toFloat();
           juce::Point<float> controlPoint1(startPoint.getX(), (startPoint.getY() + endPoint.getY()) / 2);
           juce::Point<float> controlPoint2(endPoint.getX(), (startPoint.getY() + endPoint.getY()) / 2);
        curvePath.startNewSubPath(startPoint);
        curvePath.cubicTo(controlPoint1, controlPoint2, endPoint);

           // Create hit test areas after updating the path
           createHitTestAreas(curvePath);
       }

    void createHitTestAreas(const juce::Path& curvePath) {
        hitTestAreas.clear();
        
        juce::PathFlatteningIterator iterator(curvePath, getTransform());
        while (iterator.next()) {
            float width = std::max(std::abs(iterator.x1 - iterator.x2), 1.0f);
            float height = std::max(std::abs(iterator.y1 - iterator.y2), 1.0f);
            juce::Rectangle<float> segmentRect(
                std::min(iterator.x1, iterator.x2),
                std::min(iterator.y1, iterator.y2),
                width,
                height
            );
            
//            std::cout<<"segmentRect : "<<segmentRect.toString()<<std::endl;
            
            hitTestAreas.push_back(segmentRect.expanded(5.0f));
        }
    }

    juce::Rectangle<int> calculateLineBoundsRelativeTo(juce::Component* relativeToComponent) {
        auto startPoint = startPort->getCentreRelativeTo(relativeToComponent);
        auto endPoint = endPort->getCentreRelativeTo(relativeToComponent);
        return juce::Rectangle<int>(std::min(startPoint.getX(), endPoint.getX()),
                                    std::min(startPoint.getY(), endPoint.getY()),
                                    std::abs(startPoint.getX() - endPoint.getX()),
                                    std::abs(startPoint.getY() - endPoint.getY())).expanded(5);
    }
};
