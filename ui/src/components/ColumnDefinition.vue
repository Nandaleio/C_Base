<script lang="ts" setup>
import { Button } from '@/components/ui/button'
import { Input } from '@/components/ui/input';
import {
    Select,
    SelectContent,
    SelectItem,
    SelectTrigger,
    SelectValue,
} from '@/components/ui/select'
import { Toggle } from '@/components/ui/toggle'
import {
    Popover,
    PopoverContent,
    PopoverTrigger,
} from '@/components/ui/popover'

import { Trash2, Settings, Ban, Key, ArrowUp10 } from 'lucide-vue-next'
import type { Cols } from '@/utils/types';
import {
  Tooltip,
  TooltipContent,
  TooltipProvider,
  TooltipTrigger,
} from '@/components/ui/tooltip'



defineProps<{
    col: Cols
}>();

const emit = defineEmits<{
    delete: []
}>();

</script>

<template>
    <Input type="text" name="tableName" class="rounded-e-none" placeholder="Column name" v-model="col.name" />
    <Select v-model="col.type">
        <SelectTrigger class="rounded-none">
            <SelectValue placeholder="Column type" />
        </SelectTrigger>
        <SelectContent>
            <SelectItem value="INTEGER">Integer</SelectItem>
            <SelectItem value="REAL">Real</SelectItem>
            <SelectItem value="TEXT">Text</SelectItem>
            <SelectItem value="BLOB">File</SelectItem>
        </SelectContent>
    </Select>
    <Popover>
        <PopoverTrigger>
            <Button variant="outline"  class="h-10 rounded-none">
                <Settings/>
            </Button>
        </PopoverTrigger>
        <PopoverContent >
            <div class="flex gap-3">
            <TooltipProvider>
                
                <Toggle variant="outline" v-bind:pressed="!!col.options.primary" @update:pressed="(v) => col.options.primary = v">
                    <Tooltip>
                        <TooltipTrigger as-child>
                                <Key />
                        </TooltipTrigger>
                        <TooltipContent>
                            <p>Primary Key</p>
                        </TooltipContent>
                    </Tooltip>
                </Toggle>

                <Toggle variant="outline" v-if="col.type === 'INTEGER'" v-bind:pressed="!!col.options.autoincrement" @update:pressed="(v) => col.options.autoincrement = v">
                    <Tooltip>
                        <TooltipTrigger as-child>
                                <ArrowUp10 />
                        </TooltipTrigger>
                        <TooltipContent>
                            <p>Auto Increment</p>
                        </TooltipContent>
                    </Tooltip>
                </Toggle>

                <Toggle variant="outline" v-bind:pressed="!!col.options.notnull" @update:pressed="(v) => col.options.notnull = v">
                    <Tooltip>
                        <TooltipTrigger as-child>
                                <Ban />
                        </TooltipTrigger>
                        <TooltipContent>
                            <p>Not NULL</p>
                        </TooltipContent>
                    </Tooltip>
                </Toggle>

                <Input type="text" placeholder="Default value" v-model="col.options.default"/>
                
            </TooltipProvider>
            </div>
        </PopoverContent>
    </Popover>
    <Button variant="destructive" class="h-10 rounded-s-none" @click="emit('delete')">
        <Trash2 />
    </Button>

</template>