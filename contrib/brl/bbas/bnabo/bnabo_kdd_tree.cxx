/*

Copyright (c) 2010--2011, Stephane Magnenat, ASL, ETHZ, Switzerland
You can contact the author at <stephane at magnenat dot net>

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ETH-ASL BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <iostream>
#include <stdexcept>
#include <limits>
#include <queue>
#include <algorithm>
#include <utility>
#include <sstream>
#include <cassert>
#include "bnabo_private.h"
#include "bnabo_index_heap.h"
// eliminate dependency on boost
//#include <boost/numeric/conversion/bounds.hpp>
//#include <boost/limits.hpp>
//#include <boost/format.hpp>
#ifdef HAVE_OPENMP
#include <omp.h>
#endif

/*!        \file kdtree_cpu.cpp
        \brief kd-tree search, cpu implementation
        \ingroup private
*/

namespace Nabo
{
        //! \ingroup private
        //@{
        using namespace std;
        //! Return the number of bits required to store a value
        /** \param v value to store
         * \return number of bits required
         */
        template<typename T>
        T getStorageBitCount(T v)
        {
                for (T i = 0; i < 64; ++i)
                {
                        if (v == 0)
                                return i;
                        v >>= 1;
                }
                return 64;
        }
        //! Return the index of the maximum value of a vector of positive values
        /** \param v vector of positive values
         * \return index of maximum value, 0 if the vector is empty
         */
        template<typename T, typename CloudType>
        size_t argMax(const typename NearestNeighbourSearch<T, CloudType>::Vector& v)
        {
                T maxVal(0);
                size_t maxIdx(0);
                for (int i = 0; i < static_cast<int>(v.size()); ++i)
                {
                        if (v[i] > maxVal)
                        {
                                maxVal = v[i];
                                maxIdx = i;
                        }
                }
                return maxIdx;
        }
        // OPT
        template<typename T, typename Heap, typename CloudType>
        pair<T,T> KDTreeUnbalancedPtInLeavesImplicitBoundsStackOpt<T, Heap, CloudType>::getBounds(const BuildPointsIt first, const BuildPointsIt last, const unsigned dim)
        {
                // eliminate dependency on boost
                //T minVal(boost::numeric::bounds<T>::highest());
                T minVal(numeric_limits<T>::max());
                //T maxVal(boost::numeric::bounds<T>::lowest());
                T maxVal(-numeric_limits<T>::max());
                for (BuildPointsCstIt it(first); it != last; ++it)
                {
                        //const T val(cloud.coeff(dim, *it));
                        const T val = cloud.get(dim, *it);
                        minVal = min(val, minVal);
                        maxVal = max(val, maxVal);
                }
                return make_pair(minVal, maxVal);
        }
        template<typename T, typename Heap, typename CloudType>
        unsigned KDTreeUnbalancedPtInLeavesImplicitBoundsStackOpt<T, Heap, CloudType>::buildNodes(const BuildPointsIt first, const BuildPointsIt last, const Vector minValues, const Vector maxValues)
        {
                const int count(static_cast<int>(last - first));
                assert(count >= 1);
                const auto pos(static_cast<unsigned>(nodes.size()));
                //cerr << count << endl;
                if (count <= int(bucketSize))
                {
                        const auto initBucketsSize(static_cast<uint32_t>(buckets.size()));
                        //cerr << "creating bucket with " << count << " values" << endl;
                        for (int i = 0; i < count; ++i)
                        {
                                const Index index(*(first+i));
                                assert(index < static_cast<Index>(cloud.cols()));
                                //buckets.push_back(BucketEntry(&cloud.coeff(0, index), index));
                                buckets.push_back(BucketEntry(&cloud[0][index], index));
                                //cerr << "  " << &cloud.coeff(0, index) << ", " << index << endl;
                        }
                        //cerr << "at address " << bucketStart << endl;
                        nodes.push_back(Node(createDimChildBucketSize(dim, count),initBucketsSize));
                        return pos;
                }
                // find the largest dimension of the box
                const auto cutDim = static_cast<unsigned>(argMax<T, CloudType>(maxValues - minValues));
                const T idealCutVal((maxValues(cutDim) + minValues(cutDim))/2);
                // get bounds from actual points
                const pair<T,T> minMaxVals(getBounds(first, last, cutDim));
                // correct cut following bounds
                T cutVal;
                if (idealCutVal < minMaxVals.first)
                        cutVal = minMaxVals.first;
                else if (idealCutVal > minMaxVals.second)
                        cutVal = minMaxVals.second;
                else
                        cutVal = idealCutVal;
                int l(0);
                int r(count-1);
                // partition points around cutVal
                while (true)
                {
                  //while (l < count && cloud.coeff(cutDim, *(first+l)) < cutVal)
                  while (l < count && cloud.get(cutDim, *(first+l)) < cutVal)
                    ++l;
                  //while (r >= 0 && cloud.coeff(cutDim, *(first+r)) >= cutVal)
                  while (r >= 0 && cloud.get(cutDim, *(first+r)) >= cutVal)
                    --r;
                  if (l > r)
                    break;
                  swap(*(first+l), *(first+r));
                  ++l; --r;
                }
                const int br1 = l;        // now: points[0..br1-1] < cutVal <= points[br1..count-1]
                r = count-1;
                // partition points[br1..count-1] around cutVal
                while (true)
                {
                  //while (l < count && cloud.coeff(cutDim, *(first+l)) <= cutVal)
                  while (l < count && cloud.get(cutDim, *(first+l)) <= cutVal)
                  ++l;
                  //while (r >= br1 && cloud.coeff(cutDim, *(first+r)) > cutVal)
                  while (r >= br1 && cloud.get(cutDim, *(first+r)) > cutVal)
                  --r;
                  if (l > r)
                    break;
                  swap(*(first+l), *(first+r));
                  ++l; --r;
                }
                const int br2 = l; // now: points[br1..br2-1] == cutVal < points[br2..count-1]
                // find best split index
                int leftCount;
                if (idealCutVal < minMaxVals.first)
                        leftCount = 1;
                else if (idealCutVal > minMaxVals.second)
                        leftCount = count-1;
                else if (br1 > count / 2)
                        leftCount = br1;
                else if (br2 < count / 2)
                        leftCount = br2;
                else
                        leftCount = count / 2;
                assert(leftCount > 0);
                /*if (leftCount >= count)
                {
                        cerr << "Error found in kdtree:" << endl;
                        cerr << "cloud size: " << cloud.cols() << endl;
                        cerr << "count:" << count << endl;
                        cerr << "leftCount: " << leftCount << endl;
                        cerr << "br1: " << br1 << endl;
                        cerr << "br2: " << br2 << endl;
                        cerr << "idealCutVal: " << idealCutVal << endl;
                        cerr << "cutVal: " << cutVal << endl;
                        cerr << "minMaxVals.first: " << minMaxVals.first << endl;
                        cerr << "minMaxVals.second: " << minMaxVals.second << endl;
                }*/
                assert(leftCount < count);
                // update bounds for left
                Vector leftMaxValues(maxValues);
                leftMaxValues[cutDim] = cutVal;
                // update bounds for right
                Vector rightMinValues(minValues);
                rightMinValues[cutDim] = cutVal;
                // add this
                nodes.push_back(Node(0, cutVal));
                // recurse
                const unsigned _UNUSED leftChild = buildNodes(first, first + leftCount, minValues, leftMaxValues);
                assert(leftChild == pos + 1);
                const unsigned rightChild = buildNodes(first + leftCount, last, rightMinValues, maxValues);
                // write right child index and return
                nodes[pos].dimChildBucketSize = createDimChildBucketSize(cutDim, rightChild);
                return pos;
        }

        template<typename T, typename Heap, typename CloudType>
        KDTreeUnbalancedPtInLeavesImplicitBoundsStackOpt<T, Heap, CloudType>::KDTreeUnbalancedPtInLeavesImplicitBoundsStackOpt(const CloudType& cloud, const Index dim, const unsigned creationOptionFlags, const Parameters<unsigned>& additionalParameters):
                NearestNeighbourSearch<T, CloudType>::NearestNeighbourSearch(cloud, dim, creationOptionFlags),
                //bucketSize(additionalParameters.get<unsigned>("bucketSize", 8)),
        bucketSize(additionalParameters.get("bucketSize", 8)),
                dimBitCount(getStorageBitCount<uint32_t>(this->dim)),
                dimMask((1<<dimBitCount)-1)
        {
                stringstream ss;
                if (bucketSize < 2){
                        ss << "Requested bucket size " << bucketSize << " but must be larger than 2" << std::ends;
                        throw runtime_error(ss.str().c_str());
                        //throw runtime_error((boost::format("Requested bucket size %1%, but must be larger than 2") % bucketSize).str());
              }
                if (cloud.cols() <= bucketSize)
                {
                        // make a single-bucket tree
                        for (int i = 0; i < static_cast<int>(cloud.cols()); ++i)
                          //buckets.push_back(BucketEntry(&cloud.coeff(0, i), i));
                          buckets.push_back(BucketEntry(&cloud[0][i], i));
                          nodes.push_back(Node(createDimChildBucketSize(this->dim, cloud.cols()),uint32_t(0)));
                        return;
                }
                const uint64_t maxNodeCount((0x1ULL << (32-dimBitCount)) - 1);
                const uint64_t estimatedNodeCount(cloud.cols() / (bucketSize / 2));
                if (estimatedNodeCount > maxNodeCount)
                {
                        ss << "Cloud has a risk to have more nodes " <<estimatedNodeCount << " than the kd-tree allows " << maxNodeCount<< " . The kd-tree has "
                           << dimBitCount << " bits for dimensions and " << 32-dimBitCount << " bits for node indices" << std::ends;
                        throw runtime_error(ss.str().c_str());
                }
                // build point vector and compute bounds
                BuildPoints buildPoints;
                buildPoints.reserve(cloud.cols());
                for (int i = 0; i < static_cast<int>(cloud.cols()); ++i)
                {
                        //const Vector& v(cloud.block(0,i,this->dim,1));
                        const Vector& v = cloud.get_column(i);
                        buildPoints.push_back(i);
#ifdef EIGEN3_API
                        const_cast<Vector&>(minBound) = minBound.array().min(v.array());
                        const_cast<Vector&>(maxBound) = maxBound.array().max(v.array());
#else // vnl
                        // convert to vnl -- JLM
                        //const_cast<Vector&>(minBound) = minBound.cwise().min(v);
                        //const_cast<Vector&>(maxBound) = maxBound.cwise().max(v);
                        for(int j = 0; j<dim; ++j){
                                const_cast<Vector&>(minBound)[j]=std::min(minBound[j], v[j]);
                                const_cast<Vector&>(maxBound)[j]=std::max(maxBound[j], v[j]);
                        }
#endif // EIGEN3_API
                }
                // create nodes
                buildNodes(buildPoints.begin(), buildPoints.end(), minBound, maxBound);
                buildPoints.clear();
        }
        template<typename T, typename Heap, typename CloudType>
        unsigned long KDTreeUnbalancedPtInLeavesImplicitBoundsStackOpt<T, Heap, CloudType>::knn(const Matrix& query, IndexMatrix& indices, Matrix& dists2, const Index k, const T epsilon, const unsigned optionFlags, const T maxRadius) const
        {
                checkSizesKnn(query, indices, dists2, k, optionFlags);
                const bool allowSelfMatch((optionFlags & NearestNeighbourSearch<T>::ALLOW_SELF_MATCH)!=0);
                const bool sortResults((optionFlags & NearestNeighbourSearch<T>::SORT_RESULTS)!=0);
                const bool collectStatistics((creationOptionFlags & NearestNeighbourSearch<T>::TOUCH_STATISTICS)!=0);
                const T maxRadius2(maxRadius * maxRadius);
                const T maxError2((1+epsilon)*(1+epsilon));
                const int colCount(query.cols());
                assert(nodes.size() > 0);

                IndexMatrix result(k, query.cols());
                unsigned long leafTouchedCount(0);

#pragma omp parallel
                {
                Heap heap(k);
                std::vector<T> off(dim, 0);

#pragma omp for reduction(+:leafTouchedCount) schedule(guided,32)
                for (int i = 0; i < colCount; ++i)
                {
                        leafTouchedCount += onePointKnn(query, indices, dists2, i, heap, off, maxError2, maxRadius2, allowSelfMatch, collectStatistics, sortResults);
                }
                }
                return leafTouchedCount;
        }
        template<typename T, typename Heap, typename CloudType>
        unsigned long KDTreeUnbalancedPtInLeavesImplicitBoundsStackOpt<T, Heap, CloudType>::knn(const Matrix& query, IndexMatrix& indices, Matrix& dists2, const Vector& maxRadii, const Index k, const T epsilon, const unsigned optionFlags) const
        {
                checkSizesKnn(query, indices, dists2, k, optionFlags, &maxRadii);
                const bool allowSelfMatch((optionFlags & NearestNeighbourSearch<T>::ALLOW_SELF_MATCH)!=0);
                const bool sortResults((optionFlags & NearestNeighbourSearch<T>::SORT_RESULTS)!=0);
                const bool collectStatistics((creationOptionFlags & NearestNeighbourSearch<T>::TOUCH_STATISTICS)!=0);
                const T maxError2((1+epsilon)*(1+epsilon));
                const int colCount(query.cols());
                assert(nodes.size() > 0);
                IndexMatrix result(k, query.cols());
                unsigned long leafTouchedCount(0);

#pragma omp parallel
                {
                Heap heap(k);
                std::vector<T> off(dim, 0);
#pragma omp for reduction(+:leafTouchedCount) schedule(guided,32)
                for (int i = 0; i < colCount; ++i)
                {
                        const T maxRadius(maxRadii[i]);
                        const T maxRadius2(maxRadius * maxRadius);
                        leafTouchedCount += onePointKnn(query, indices, dists2, i, heap, off, maxError2, maxRadius2, allowSelfMatch, collectStatistics, sortResults);
                }
                }
                return leafTouchedCount;
        }
        template<typename T, typename Heap, typename CloudType>
        unsigned long KDTreeUnbalancedPtInLeavesImplicitBoundsStackOpt<T, Heap, CloudType>::onePointKnn(const Matrix& query, IndexMatrix& indices, Matrix& dists2, int i, Heap& heap, std::vector<T>& off, const T maxError2, const T maxRadius2, const bool allowSelfMatch, const bool collectStatistics, const bool sortResults) const
        {
                fill(off.begin(), off.end(), 0);
                heap.reset();
                unsigned long leafTouchedCount(0);
                if (allowSelfMatch)
                {
                        if (collectStatistics)
                          //leafTouchedCount += recurseKnn<true, true>(&query.coeff(0, i), 0, 0, heap, off, maxError2, maxRadius2);
                          leafTouchedCount += recurseKnn<true, true>(&query[0][i], 0, 0, heap, off, maxError2, maxRadius2);
                        else
                          //recurseKnn<true, false>(&query.coeff(0, i), 0, 0, heap, off, maxError2, maxRadius2);
                          recurseKnn<true, false>(&query[0][i], 0, 0, heap, off, maxError2, maxRadius2);
                }
                else
                {
                  if (collectStatistics)
                    //   leafTouchedCount += recurseKnn<false, true>(&query.coeff(0, i), 0, 0, heap, off, maxError2, maxRadius2);
                    leafTouchedCount += recurseKnn<false, true>(&query[0][i], 0, 0, heap, off, maxError2, maxRadius2);
                  else
                    //recurseKnn<false, false>(&query.coeff(0, i), 0, 0, heap, off, maxError2, maxRadius2);
                    recurseKnn<false, false>(&query[0][i], 0, 0, heap, off, maxError2, maxRadius2);
                }
                // does nothing
                if (sortResults)
                        heap.sort();
                //heap.getData(indices.col(i), dists2.col(i));
                // adapt to vnl interface
                vnl_vector<Index> indx(indices.rows());
                vnl_vector<T> dsts(dists2.rows());
                heap.getData(indx, dsts);
                indices.set_column(i,indx); dists2.set_column(i,dsts);
                return leafTouchedCount;
        }
        template<typename T, typename Heap, typename CloudType> template<bool allowSelfMatch, bool collectStatistics>
        unsigned long KDTreeUnbalancedPtInLeavesImplicitBoundsStackOpt<T, Heap, CloudType>::recurseKnn(const T* query, const unsigned n, T rd, Heap& heap, std::vector<T>& off, const T maxError2, const T maxRadius2) const
        {
                const Node& node(nodes[n]);
                const uint32_t cd(getDim(node.dimChildBucketSize));
                if (cd == uint32_t(dim))
                {
                        //cerr << "entering bucket " << node.bucket << endl;
                        const BucketEntry* bucket(&buckets[node.bucketIndex]);
                        const uint32_t bucketSize(getChildBucketSize(node.dimChildBucketSize));
                        for (uint32_t i = 0; i < bucketSize; ++i)
                        {
                                //cerr << "  " << bucket-> pt << endl;
                                //const T dist(dist2<T>(query, cloud.col(index)));
                                //const T dist((query - cloud.col(index)).squaredNorm()); (commented out in original nabo, for speed? JLM)
                                auto ncols = static_cast<unsigned>(cloud.cols());
                                T dist(0);
                                const T* qPtr(query);
                                const T* dPtr(bucket->pt);
                                for (unsigned i = 0; i < static_cast<unsigned>(this->dim); ++i)
                                {
                                        const T diff(*qPtr-*dPtr);
                                        dist += diff*diff;
                                        qPtr++; dPtr+=ncols; //vnl matrices are row major not column major as assumed by nabo
                                }
                                if ((dist <= maxRadius2) &&
                                        (dist < heap.headValue()) &&
                                        (allowSelfMatch || (dist > numeric_limits<T>::epsilon()))
                                )
                                heap.replaceHead(bucket->index, dist);
                                ++bucket;
                        }
                        return (unsigned long)(bucketSize);
                }
                else
                {
                        const unsigned rightChild(getChildBucketSize(node.dimChildBucketSize));
                        unsigned long leafVisitedCount(0);
                        T& offcd(off[cd]);
                        //const T old_off(off.coeff(cd));
                        const T old_off(offcd);
                        const T new_off(query[cd] - node.cutVal);
                        if (new_off > 0)
                        {
                                if (collectStatistics)
                                        leafVisitedCount += recurseKnn<allowSelfMatch, true>(query, rightChild, rd, heap, off, maxError2, maxRadius2);
                                else
                                        recurseKnn<allowSelfMatch, false>(query, rightChild, rd, heap, off, maxError2, maxRadius2);
                                rd += - old_off*old_off + new_off*new_off;
                                if ((rd <= maxRadius2) &&
                                        (rd * maxError2 < heap.headValue()))
                                {
                                        offcd = new_off;
                                        if (collectStatistics)
                                                leafVisitedCount += recurseKnn<allowSelfMatch, true>(query, n + 1, rd, heap, off, maxError2, maxRadius2);
                                        else
                                                recurseKnn<allowSelfMatch, false>(query, n + 1, rd, heap, off, maxError2, maxRadius2);
                                        offcd = old_off;
                                }
                        }
                        else
                        {
                                if (collectStatistics)
                                        leafVisitedCount += recurseKnn<allowSelfMatch, true>(query, n+1, rd, heap, off, maxError2, maxRadius2);
                                else
                                        recurseKnn<allowSelfMatch, false>(query, n+1, rd, heap, off, maxError2, maxRadius2);
                                rd += - old_off*old_off + new_off*new_off;
                                if ((rd <= maxRadius2) &&
                                        (rd * maxError2 < heap.headValue()))
                                {
                                        offcd = new_off;
                                        if (collectStatistics)
                                                leafVisitedCount += recurseKnn<allowSelfMatch, true>(query, rightChild, rd, heap, off, maxError2, maxRadius2);
                                        else
                                                recurseKnn<allowSelfMatch, false>(query, rightChild, rd, heap, off, maxError2, maxRadius2);
                                        offcd = old_off;
                                }
                        }
                        return leafVisitedCount;
                }
        }
        template struct KDTreeUnbalancedPtInLeavesImplicitBoundsStackOpt<float,IndexHeapSTL<int,float> >;
        template struct KDTreeUnbalancedPtInLeavesImplicitBoundsStackOpt<float,IndexHeapBruteForceVector<int,float> >;
        template struct KDTreeUnbalancedPtInLeavesImplicitBoundsStackOpt<double,IndexHeapSTL<int,double> >;
        template struct KDTreeUnbalancedPtInLeavesImplicitBoundsStackOpt<double,IndexHeapBruteForceVector<int,double> >;
#if 0        // no longer based on Eigen now vnl
        template struct KDTreeUnbalancedPtInLeavesImplicitBoundsStackOpt<float,IndexHeapSTL<int,float>,Eigen::Matrix3Xf>;
        template struct KDTreeUnbalancedPtInLeavesImplicitBoundsStackOpt<float,IndexHeapBruteForceVector<int,float>,Eigen::Matrix3Xf>;
        template struct KDTreeUnbalancedPtInLeavesImplicitBoundsStackOpt<double,IndexHeapSTL<int,double>,Eigen::Matrix3Xd>;
        template struct KDTreeUnbalancedPtInLeavesImplicitBoundsStackOpt<double,IndexHeapBruteForceVector<int,double>,Eigen::Matrix3Xd>;

        template struct KDTreeUnbalancedPtInLeavesImplicitBoundsStackOpt<float,IndexHeapSTL<int,float>,Eigen::Map<const Eigen::Matrix3Xf, Eigen::Aligned> >;
        template struct KDTreeUnbalancedPtInLeavesImplicitBoundsStackOpt<float,IndexHeapBruteForceVector<int,float>,Eigen::Map<const Eigen::Matrix3Xf, Eigen::Aligned> >;
        template struct KDTreeUnbalancedPtInLeavesImplicitBoundsStackOpt<double,IndexHeapSTL<int,double>,Eigen::Map<const Eigen::Matrix3Xd, Eigen::Aligned> >;
        template struct KDTreeUnbalancedPtInLeavesImplicitBoundsStackOpt<double,IndexHeapBruteForceVector<int,double>,Eigen::Map<const Eigen::Matrix3Xd, Eigen::Aligned> >;
#endif
        //@}
}
