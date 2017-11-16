#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace enc = sensor_msgs::image_encodings;

class MYNTEYEListener {
public:
    MYNTEYEListener() : it_(nh_), color_count_(0) {
        color_sub_ =  it_.subscribe("mynteye/color", 100, &MYNTEYEListener::colorCallback, this);
        depth_sub_ =  it_.subscribe("mynteye/depth", 100, &MYNTEYEListener::depthCallback, this);

        cv::namedWindow("color", cv::WINDOW_AUTOSIZE);
        cv::namedWindow("depth", cv::WINDOW_AUTOSIZE);
    }

    ~MYNTEYEListener() {
        cv::destroyAllWindows();
    }

    void colorCallback(const sensor_msgs::ImageConstPtr &msg) {
        cv_bridge::CvImageConstPtr cv_ptr;
        try {
            cv_ptr = cv_bridge::toCvShare(msg, enc::BGR8);
        } catch (cv_bridge::Exception &e) {
            ROS_ERROR_STREAM("cv_bridge exception: " << e.what());
            return;
        }
        cv::imshow("color", cv_ptr->image);
        cv::waitKey(3);
    }

    void depthCallback(const sensor_msgs::ImageConstPtr &msg) {
        cv_bridge::CvImageConstPtr cv_ptr;
        try {
            if (enc::isColor(msg->encoding)) {
                cv_ptr = cv_bridge::toCvShare(msg, enc::BGR8);
            } else {
                cv_ptr = cv_bridge::toCvShare(msg, enc::MONO8);
            }
        } catch (cv_bridge::Exception &e) {
            ROS_ERROR_STREAM("cv_bridge exception: " << e.what());
            return;
        }
        cv::imshow("depth", cv_ptr->image);
        cv::waitKey(3);
    }

    std::uint64_t colorCount() const {
        return color_count_;
    }

private:
    ros::NodeHandle nh_;
    image_transport::ImageTransport it_;
    image_transport::Subscriber color_sub_;
    image_transport::Subscriber depth_sub_;

    std::uint64_t color_count_;
};

int main(int argc, char *argv[]) {
    ros::init(argc, argv, "mynteye_listener");

    MYNTEYEListener l;

    double time_beg = ros::Time::now().toSec();
    ros::spin();
    double time_end = ros::Time::now().toSec();

    double elapsed = time_end - time_beg;
    ROS_INFO_STREAM("time beg: " << std::fixed << time_beg << " s");
    ROS_INFO_STREAM("time end: " << std::fixed << time_end << " s");
    ROS_INFO_STREAM("time cost: " << elapsed << " s");
    ROS_INFO_STREAM("color count: " << l.colorCount() << ", " << (l.colorCount() / elapsed) << " fps");

    return 0;
}