# MapReduce 

## Mappers

Mappers contain logic that transforms chunks 
into an intermediate form that can then be 
further reduced.

## Combiners

Combiners are run in-process (or in-thread) 
with mappers to pre-combine results in order 
to reduce network load.

The functionality is usually identical to this
of reducers.

## Reducers
   
Reducers contain logic that reduces the output 
of combiners into the result required for servicing.
