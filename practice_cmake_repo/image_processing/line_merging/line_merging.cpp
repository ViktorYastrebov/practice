namespace image_processing {
	namespace algorithm {	
	
		GroupLinesBuilder::GroupLinesBuilder(const double maxDistanceBetweenLines, const double maxSameLineDistance)
			: MAX_DISTANCE_BETWEEN_LINES_(maxDistanceBetweenLines)
			, MAX_SAME_LINE_DISTANCE_(maxSameLineDistance)
		{}

		std::vector<cv::Vec4i> GroupLinesBuilder::operator()(const std::vector<cv::Vec4i> &inputs) const {
			std::vector<cv::Vec4i>::const_iterator it = inputs.begin();
			std::vector<cv::Vec4i>::const_iterator e = inputs.end();

			MapType accum;
			if (!inputs.empty()) {
				accum.push_back(std::make_pair<LineParams, std::vector<cv::Vec4i>>(getLineParams(*it), {*it}) );
				++it;
			}

			for (it; it != e; ++it) {
				addToVector(accum, getLineParams(*it), *it);
			}

			std::vector<cv::Vec4i> result;
			for (auto &groupElem : accum) {
				std::sort(groupElem.second.begin(), groupElem.second.end(), [](const cv::Vec4i &line1, const cv::Vec4i &line2) -> bool {
					return line1[0] < line2[0];
				});
				const auto &minPoint = groupElem.second.front();
				const auto &maxPoint = groupElem.second.back();

				cv::Vec4i line(minPoint[0], minPoint[1], maxPoint[2], maxPoint[3]);
				result.push_back(line);
			}
			return result;
		}

		GroupLinesBuilder::LineParams ::GroupLinesBuilder::getLineParams(const cv::Vec4i &line) const {
			return {
				line[1] - line[3],
				line[2] - line[0],
				(line[0] * line[3] - line[2] * line[1])
			};
		}

		bool GroupLinesBuilder::acceptFunc(const LineParams &params1, const LineParams &params2, const Vec4i &line1, const Vec4i &line2) const {
			int ab = params1.A * params2.A + params1.B * params2.B;
			double p1Norm = sqrt(params1.A * params1.A + params1.B * params1.B);
			double p2Norm = sqrt(params2.A * params2.A + params2.B * params2.B);

			double cosAB = ab / (p1Norm * p2Norm);
			bool acceptDegree = cosAB > COS_10_DEGREE_;

			Point line1MidPoint(
				(line1[0] + line1[2]) / 2,
				(line1[1] + line1[3]) / 2
			);

			Point2i line2MidPoint(
				(line2[0] + line2[2]) / 2,
				(line2[1] + line2[3]) / 2
			);

			double distanceLine1ToLine2 = std::abs(params1.A * line2MidPoint.x + params1.B * line2MidPoint.y + params1.C) / p1Norm;
			double distanceLine2ToLine1 = std::abs(params2.A * line1MidPoint.x + params2.B * line1MidPoint.y + params2.C) / p2Norm;

			bool isAcceptedDistance = distanceLine1ToLine2 < MAX_DISTANCE_BETWEEN_LINES_ || distanceLine2ToLine1 < MAX_DISTANCE_BETWEEN_LINES_;

			//Point can lay on the same line
			// let's in addition calc the distance between the points.
			if (isAcceptedDistance) {
				double sameLineDistance = cv::norm(line1MidPoint - line2MidPoint);
				isAcceptedDistance &= sameLineDistance < MAX_SAME_LINE_DISTANCE_;
			}
			return acceptDegree && isAcceptedDistance;
		}

		GroupLinesBuilder::LineParams ::GroupLinesBuilder::generalizeLineParams(const LineParams &p1, const LineParams & p2) const {
			return { (p1.A + p2.A) / 2, (p1.B + p2.B) / 2, (p1.C + p2.C) / 2 };
		}

		void GroupLinesBuilder::addToVector(MapType &accum, const LineParams &params, const cv::Vec4i &line) const {
			MapType::iterator it = std::find_if(accum.begin(), accum.end(), [&line, &params, this](const MapType::value_type &vecData) -> bool {

				bool res = true;
				std::vector<cv::Vec4i>::const_iterator it = vecData.second.begin();
				std::vector<cv::Vec4i>::const_iterator e = vecData.second.end();
				for (; it != e && res; ++it) {
					res &= acceptFunc(params, getLineParams(*it), line, *it);
				}
				return res;
			});

			if (it != accum.end()) {
				it->second.push_back(line);
			}
			else {
				LineParams paramCopy = params;
				accum.push_back(std::make_pair<LineParams, std::vector<cv::Vec4i>>(std::move(paramCopy), { line }));
			}
		}
	}
}