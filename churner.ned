//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package symphony;

simple Churner
{
    parameters:
        volatile double join_freq @unit(s) = default(1s);
        volatile double leave_freq @unit(s) = default(3s);
        volatile int noOfJoins = default(0);
        volatile int noOfLeaves = default(0);
        @display("i=block/timer");
        
        @statistic[NSSig](record=last,max,min,mean,timeavg,vector?);
    gates:
}
